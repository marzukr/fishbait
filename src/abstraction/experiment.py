#!/usr/bin/env python3

from itertools import combinations
import time
import hand_evaluator
import OCHS
import concurrent.futures
import matplotlib.pyplot as plt
import numpy as np
import json
import sys
import os
import pickle
from pymongo import MongoClient

def rollouts(hand, roll_size=5):
    rem_cards = [i for i in range(0, 52) if i not in hand]
    return combinations(rem_cards, roll_size)

def possible_opponent_hands(hand, rollout=()):
    rem_cards = [i for i in range(0, 52) if i not in hand and i not in rollout]
    return combinations(rem_cards, 2)

# omp = hand_evaluator.OMP()
def compute_win(hero, villan, rollout):
    hero_rank = hand_evaluator.GetRank(rollout[0], rollout[1], rollout[2],
                                       rollout[3], rollout[4], 
                                       hero[0], hero[1])
    villan_rank = hand_evaluator.GetRank(rollout[0], rollout[1], rollout[2],
                                         rollout[3], rollout[4], 
                                         villan[0], villan[1])
    # hero_rank = omp.GetRank(rollout[0], rollout[1], rollout[2],
    #     rollout[3], rollout[4], hero[0], hero[1])
    # villan_rank = omp.GetRank(rollout[0], rollout[1], rollout[2],
    #     rollout[3], rollout[4], villan[0], villan[1])
    if hero_rank > villan_rank:
        return 1
    elif hero_rank < villan_rank:
        return 0
    return 0.5

def roll_strength(ins):
    hero = ins['hand']
    rollout = ins['rollout']
    iso_idx = ins['iso_idx']
    wins = 0
    total = 0
    for op_hand in possible_opponent_hands(hero, rollout):
        total += 1
        wins += compute_win(hero, op_hand, rollout)
    return {
        'hand_strength': wins / total, 
        'iso_idx': iso_idx
    }

def generate_hand_strength_roll(hand):
    isocalc = hand_evaluator.Indexer(2, [2,5])
    ins = {}
    for rollout in rollouts(hand):
        iso_idx = isocalc.index([hand[0], hand[1],
            rollout[0], rollout[1], rollout[2], rollout[3], rollout[4]], False)
        if iso_idx in ins:
            ins[iso_idx]['weight'] += 1
        else:
            ins[iso_idx] = {
                'hand': hand, 
                'rollout': rollout, 
                'weight': 1, 
                'iso_idx': iso_idx, 
                'hand_strength': None
            }

    ts = time.time()
    i = 0
    total = len(ins)

    with concurrent.futures.ProcessPoolExecutor() as executor:
        results = executor.map(roll_strength, ins.values(), chunksize=3000)
        for result in results:
            iso_idx = result['iso_idx']
            hand_strength = result['hand_strength']
            ins[iso_idx]['hand_strength'] = hand_strength
            del ins[iso_idx]['hand']
            del ins[iso_idx]['rollout']
            i += 1
            progress = i / total * 100
            if time.time() - ts >= 30:
                print('{}: {}%'.format(hand, progress))
                ts = time.time()

    return {'data': ins, 'hand': hand}

def hand_str(cards):
    ret_str = ''
    for card in cards:
        c = hand_evaluator.CardString(card)
        ret_str += ',' + c
    return ret_str[1:]

def generate_hand_strength_roll_ochs(hand, rollout, op_clusters):
    # ts = time.time()
    # r_total = 2118760
    # r = 0
    # for rollout in rollouts(hand):
    wins = np.zeros(8)
    total = np.zeros(8)
    for op_hand in possible_opponent_hands(hand, rollout):
        cluster = op_clusters[op_hand[0], op_hand[1]]
        assert(cluster != -1)
        wins[cluster] += compute_win(hand, op_hand, rollout)
        total[cluster] += 1
    return (wins/total).tolist()
        # r += 1
        # progress = r / r_total * 100
        # if time.time() - ts >= 30:
        #     print('{}: {}%'.format(hand, progress))
        #     ts = time.time()

def save_distribution(hand, res):
    jdata = json.dumps(res, indent=4)
    with open('luts/{}_{}.json'.format(hand[0], hand[1]), 'w') as f:
        f.write(jdata)
    print('saved {}'.format(hand))

def save_binary(location, res):
    max_bytes = 2**31 - 1
    bytes_out = pickle.dumps(res)
    with open(location, 'wb') as f_out:
        for idx in range(0, len(bytes_out), max_bytes):
            f_out.write(bytes_out[idx:idx+max_bytes])

def load_binary(file_path):
    max_bytes = 2**31 - 1
    bytes_in = bytearray(0)
    input_size = os.path.getsize(file_path)
    with open(file_path, 'rb') as f_in:
        for _ in range(0, input_size, max_bytes):
            bytes_in += f_in.read(max_bytes)
    data2 = pickle.loads(bytes_in)
    return data2

def load_raw_json(filename):
    data = None
    with open(filename, 'r') as f:
        data = json.load(f)
    return data

def load_distribution(mongo_res):
    dist = []
    for roll in mongo_res:
        hs = roll['hand_strength']
        weight = roll['weight']
        dist.extend([hs]*weight)
    return dist

def flop_hand_strength_manual(hand, flop):
    dist = []
    for rollout in possible_opponent_hands(hand, flop):
        hs = roll_strength({
            'hand': hand,
            'rollout': (flop[0], flop[1], flop[2], rollout[0], rollout[1]),
            'iso_idx': None
        })['hand_strength']
        dist.append(hs)
    print(sum(dist) / len(dist))
    _ = plt.hist(dist, bins=50, range=(0,1))
    plt.show()

def flop_from_precompute(hand, flop, data):
    isocalc = hand_evaluator.Indexer(2, [2,5])
    dist = []
    for rollout in possible_opponent_hands(hand, flop):
        iso_idx = str(isocalc.index([hand[0], hand[1],
            flop[0], flop[1], flop[2], rollout[0], rollout[1]], False))
        hs = data[iso_idx]['hand_strength']
        dist.append(hs)
    print(sum(dist) / len(dist))
    _ = plt.hist(dist, bins=50, range=(0,1))
    plt.show()

def generate_turn_histograms(hand):
    client = MongoClient()
    db = client.pluribus
    hand_strengths = db.hand_strengths
    turns = db.turns

    isocalc = hand_evaluator.Indexer(2, [2,5])
    turncalc = hand_evaluator.Indexer(2, [2,4])

    roll_dict = {}
    roll_list = []
    for rollout in rollouts(hand):
        roll_cards = [hand[0], hand[1], 
            rollout[0], rollout[1], rollout[2], rollout[3], rollout[4]]
        roll_idx = isocalc.index(roll_cards, False)
        if roll_idx not in roll_dict:
            roll_list.append(roll_idx)
            roll_dict[roll_idx] = True
    print("found rollouts {}".format(hand))
    result = hand_strengths.find({"_id":{"$in":roll_list}})
    print("queried db {}".format(hand))
    for res in result:
        hand_id = res["_id"]
        roll_dict[hand_id] = res["hand_strength"]
    print("loaded {}".format(hand))

    idxs = {}
    to_add = []
    for flop in rollouts(hand, roll_size=4):
        cards = [hand[0], hand[1], 
            flop[0], flop[1], flop[2], flop[3]]
        idx = turncalc.index(cards, False)
        if idx not in idxs:
            idxs[idx] = True
            hs_idxs = []
            for tr in rollouts(cards, roll_size=1):
                all_cards = [hand[0], hand[1], 
                    flop[0], flop[1], flop[2], flop[3], tr[0]]
                hs_idx = isocalc.index(all_cards, False)
                hs_idxs.append(hs_idx)
            dist = [roll_dict[i] for i in hs_idxs]
            # print(sum(dist)/len(dist))
            # _ = plt.hist(dist, bins=50, range=(0,1))
            # print(hand)
            # print(flop)
            # plt.show()
            hist = np.histogram(dist, bins=50, range=(0,1))[0].tolist()
            to_add.append({"_id": idx, "hist": hist})
    turns.insert_many(to_add)
    print("completed {}".format(hand))
    to_add = []

if __name__ == '__main__':
    assert(len(sys.argv) == 2)
    try:
        hand_num = int(sys.argv[1])
    except ValueError:
        raise
    assert(hand_num < 169)
    assert(hand_num >= 0)

    deck = [i for i in range(0, 52)]
    hands = {}
    handcalc = hand_evaluator.Indexer(1, [2])
    for i in range(0, len(deck)):
        for j in range(i + 1, len(deck)):
            hand_idx = handcalc.index([i, j], False)
            if hand_idx not in hands:
                hands[hand_idx] = (i, j)
    hands = [hand for hand in hands.values()]
    hands.sort(key=lambda tup: tup[0])
    hand = hands[hand_num]

    op_clusters = np.zeros((52,52), dtype=np.int8) - 1
    hand_indexer = hand_evaluator.Indexer(1, [2])
    for i in range(0, len(deck)):
        for j in range(0, len(deck)):
            if i != j:
                hand_index = hand_indexer.index([i, j], False)
                cluster = OCHS.op_clusters[hand_index] - 1
                op_clusters[i][j] = cluster
    
    isocalc = hand_evaluator.Indexer(2, [2,5])
    unique_rollouts = {}
    for rollout in rollouts(hand):
        iso_idx = isocalc.index([hand[0], hand[1],
            rollout[0], rollout[1], rollout[2], rollout[3], rollout[4]], 
            False)
        if iso_idx not in unique_rollouts:
            unique_rollouts[iso_idx] = rollout
            if len(unique_rollouts) > 12:
                break

    ret = {}
    with concurrent.futures.ProcessPoolExecutor(max_workers=12) as executor:
        future_to_iso_idx = {
            executor.submit(generate_hand_strength_roll_ochs, 
                hand, rollout, op_clusters): iso_idx 
            for iso_idx, rollout in unique_rollouts.items()
        }
        for future in concurrent.futures.as_completed(future_to_iso_idx):
            vect = future.result()
            iso_idx = future_to_iso_idx[future]
            ret[iso_idx] = vect
    save_binary("luts/{}_{}.ochs".format(hand[0], hand[1]), ret)

    # EMD
    # with open('QsKsISO.npy', 'rb') as f:
    #     b1 = np.load(f)
    #     b1 = plt.hist(b1, bins=50, range=(0,1))
    #     b1s = sum(b1[0])
    # with open('6s6hISO.npy', 'rb') as f:
    #     b2 = np.load(f)
    #     b2 = plt.hist(b2, bins=50, range=(0,1))
    #     b2s = sum(b2[0])
    # prev = 0
    # tot = 0
    # for i in range(1, len(b1[0])):
    #     nxt = b1[0][i-1] / b1s + prev - b2[0][i-1] / b2s
    #     tot += abs(nxt)
    #     prev = nxt
    # print(tot)

