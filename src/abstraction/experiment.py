import collections
from itertools import combinations
import time
import hand_evaluator
import concurrent.futures
from multiprocessing import Pool
import matplotlib.pyplot as plt
import numpy as np

deck = [i for i in range(0, 52)]
hands = []
for i in range(0, len(deck)):
    for j in range(i + 1, len(deck)):
        hand = (i, j)
        hands.append(hand)

def rollouts(hand, op_hand=()):
    rem_cards = [i for i in range(0, 52) if i not in hand and i not in op_hand]
    return combinations(rem_cards, 5)

def possible_opponent_hands(hand, rollout=()):
    rem_cards = [i for i in range(0, 52) if i not in hand and i not in rollout]
    return combinations(rem_cards, 2)

def compute_win(hero, villan, rollout):
    hero_rank = hand_evaluator.GetRank(rollout[0], rollout[1], rollout[2],
                                       rollout[3], rollout[4], 
                                       hero[0], hero[1])
    villan_rank = hand_evaluator.GetRank(rollout[0], rollout[1], rollout[2],
                                         rollout[3], rollout[4], 
                                         villan[0], villan[1])
    # hero_rank = hand_evaluator.GetRankOMP(rollout[0], rollout[1], rollout[2],
    #                                       rollout[3], rollout[4], 
    #                                       hero[0], hero[1])
    # villan_rank = hand_evaluator.GetRankOMP(rollout[0], rollout[1], rollout[2],
    #                                         rollout[3], rollout[4], 
    #                                         villan[0], villan[1])
    if hero_rank > villan_rank:
        return 1
    elif hero_rank < villan_rank:
        return 0
    return 0.5

def opponent_strength(ins):
    hero = ins[0]
    villan = ins[1]
    wins = 0
    total = 0
    for rollout in rollouts(hero, villan):
        total += 1
        wins += compute_win(hero, villan, rollout)
    return (wins / total, wins, total)

def generate_hand_strength_op(hand):
    ts = time.time()
    distribution = []
    total = sum(1 for _ in possible_opponent_hands(hand))
    i = 0
    wins = 0
    games = 0
    ins = [(hand, op_hand) for op_hand in possible_opponent_hands(hand)]
    with concurrent.futures.ProcessPoolExecutor() as executor:
        # futures = {executor.submit(rollout_strength, ini): ini for ini in ins}
        results = executor.map(opponent_strength, ins)
        # for future in concurrent.futures.as_completed(futures):
        #     print(future.result())
        for result in results:
            distribution.append(result[0])
            wins += result[1]
            games += result[2]
            i += 1
            progress = i / total * 100
            if time.time() - ts >= 60:
                print(progress)
                ts = time.time()

    return (distribution, wins / games)

def roll_strength(ins):
    hero = ins[0]
    rollout = ins[1]
    wins = 0
    total = 0
    for op_hand in possible_opponent_hands(hero, rollout):
        total += 1
        wins += compute_win(hero, op_hand, rollout)
    return (wins / total, wins, total)

def generate_hand_strength_roll(hand):
    ts = time.time()
    distribution = []
    total = sum(1 for _ in rollouts(hand))
    i = 0
    wins = 0
    games = 0
    ins = [(hand, rollout) for rollout in rollouts(hand)]
    # for inputs in ins[2118000:]:
    #     result = roll_strength(inputs)
    #     distribution.append(result[0])
    #     wins += result[1]
    #     games += result[2]
    #     i += 1
    #     progress = i / total * 100
    #     if time.time() - ts >= 60:
    #         print(progress)
    #         ts = time.time()

    with concurrent.futures.ProcessPoolExecutor() as executor:
        # futures = {executor.submit(rollout_strength, ini): ini for ini in ins}
        results = executor.map(roll_strength, ins, chunksize=900)
        # for future in concurrent.futures.as_completed(futures):
        #     print(future.result())
        for result in results:
            distribution.append(result[0])
            wins += result[1]
            games += result[2]
            i += 1
            progress = i / total * 100
            if time.time() - ts >= 60:
                print(progress)
                ts = time.time()

    return (distribution, wins / games)

if __name__ == '__main__':
    # res = generate_hand_strength_roll((4, 8)) # QsKs seveneval
    # res = generate_hand_strength_roll((32, 33)) # 6s6h seveleval
    # res = generate_hand_strength_roll((40, 41)) # 4s4h seveleval
    # res = generate_hand_strength_roll((12, 16)) # TsJs seveleval
    # res = generate_hand_strength_roll((40, 44)) # QsKs OMP
    # dst = res[0] 
    # print(sum(dst) / len(dst))
    # print(res[1])
    # with open('TsJs.npy', 'wb') as f:
    #     np.save(f, dst)
    # _ = plt.hist(dst, bins=50)
    # plt.show()

    # EMD
    with open('TsJs.npy', 'rb') as f:
        b1 = np.load(f)
        b1 = plt.hist(b1, bins=50, range=(0,1))
        b1s = sum(b1[0])
    with open('4s4h.npy', 'rb') as f:
        b2 = np.load(f)
        b2 = plt.hist(b2, bins=50, range=(0,1))
        b2s = sum(b2[0])
    prev = 0
    tot = 0
    for i in range(1, len(b1[0])):
        nxt = b1[0][i-1] / b1s + prev - b2[0][i-1] / b2s
        tot += abs(nxt)
        prev = nxt
    print(tot)
