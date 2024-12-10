#ifndef AI_SRC_RELAY_SCRIBE_H_
#define AI_SRC_RELAY_SCRIBE_H_

#include <array>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "array/array.h"
#include "H5Cpp.h"
#include "mccfr/definitions.h"
#include "mccfr/sequence_table.h"
#include "mccfr/strategy.h"
#include "poker/definitions.h"
#include "utils/cereal.h"

namespace fishbait {

/*
  This class saves all the information neccecary to run the blueprint strategy
  (the information in Strategy::Average) to an HDF5 file and exposes functions
  to read the neccecary information from it.
*/
template <PlayerId kPlayers, std::size_t kActions, typename InfoAbstraction>
class Scribe {
 private:
  H5::H5File average_;  // hdf file to read/write the strategy from/to

  H5::EnumType PlayType() {
    H5::EnumType play_type{H5::PredType::NATIVE_UINT8};
    uint8_t play_val = 0;
    play_type.insert("kFold", &play_val);
    play_val = 1;
    play_type.insert("kCheckCall", &play_val);
    play_val = 2;
    play_type.insert("kBet", &play_val);
    play_val = 3;
    play_type.insert("kAllIn", &play_val);
    return play_type;
  }  // PlayType()

  H5::EnumType RoundType() {
    H5::EnumType round_type{H5::PredType::NATIVE_UINT8};
    uint8_t round_val = 0;
    round_type.insert("kPreFlop", &round_val);
    round_val = 1;
    round_type.insert("kFlop", &round_val);
    round_val = 2;
    round_type.insert("kTurn", &round_val);
    round_val = 3;
    round_type.insert("kRiver", &round_val);
    return round_type;
  }  // RoundType()

  H5::CompType AbstractActionType() {
    H5::EnumType play_type = PlayType();
    H5::EnumType round_type = RoundType();

    H5::CompType abstract_action_type{sizeof(AbstractAction)};
    abstract_action_type.insertMember("play", HOFFSET(AbstractAction, play),
                                      play_type);
    abstract_action_type.insertMember("size", HOFFSET(AbstractAction, size),
                                      H5::PredType::NATIVE_DOUBLE);
    abstract_action_type.insertMember("max_raise_num",
                                      HOFFSET(AbstractAction, max_raise_num),
                                      H5::PredType::NATIVE_INT);
    abstract_action_type.insertMember("min_round",
                                      HOFFSET(AbstractAction, min_round),
                                      round_type);
    abstract_action_type.insertMember("max_round",
                                      HOFFSET(AbstractAction, max_round),
                                      round_type);
    abstract_action_type.insertMember("max_players",
                                      HOFFSET(AbstractAction, max_players),
                                      H5::PredType::NATIVE_UINT8);
    abstract_action_type.insertMember("min_pot",
                                      HOFFSET(AbstractAction, min_pot),
                                      H5::PredType::NATIVE_UINT32);
    return abstract_action_type;
  }  // AbstractActionType()

  /*
    @brief Saves the given Node as a json dump to the root of the hdf file.
  */
  void SaveStartState(const Node<kPlayers>& start_state) {
    std::stringstream ss_ss;
    CerealSaveJSON(ss_ss, &start_state);
    std::string ss_string = ss_ss.str();

    H5::DataSpace fspace{H5S_SCALAR};
    H5::StrType ftype{H5::PredType::C_S1, ss_string.length() + 1};
    H5::DataSet dataset = average_.createDataSet("start_state", ftype, fspace);
    dataset.write(ss_string, ftype);
  }  // SaveStartState()

  /*
    @brief Creates a hdf group with a single dataset called "index".

    This dataset is an array of references, one for each poker round.

    @param name What to name the group.
  */
  H5::Group CreateIndexedDSetGroup(std::string_view name) {
    H5::Group group = average_.createGroup(name.data());
    std::array<hsize_t, 1> index_fspace_dims = {kNRounds};
    H5::DataSpace index_fspace{1, index_fspace_dims.data()};
    H5::DataSet index_dataset = group.createDataSet("index",
        H5::PredType::STD_REF_OBJ, index_fspace);
    return group;
  }  // CreateIndexedDSetGroup()

  /*
    @brief Writes a reference from the given dataset into the given group.

    The reference is written at the rid'th element of the group's "index"
    dataset.

    @param group The group where the "index" dataset can be found.
    @param round_dset The dataset to refer to.
    @param rid The element of the group's "index" dataset where we should write
        the reference.
  */
  void WriteRefToIndexDSet(const H5::Group& group,
                           const H5::DataSet& round_dset, RoundId rid) {
    void* round_ref = nullptr;
    round_dset.reference(&round_ref, round_dset.getObjName());

    hsize_t coord = rid;
    std::array<hsize_t, 1> index_fspace_dims = {kNRounds};
    H5::DataSpace index_fspace{1, index_fspace_dims.data()};
    index_fspace.selectElements(H5S_SELECT_SET, 1, &coord);
    H5::DataSet idx_dset = group.openDataSet("index");

    std::array<hsize_t, 1> round_index_mspace_dims = {1};
    H5::DataSpace round_index_mspace{1, round_index_mspace_dims.data()};
    idx_dset.write(&round_ref, H5::PredType::STD_REF_OBJ,
                   round_index_mspace, index_fspace);
  }  // WriteRefToIndexDSet

  /*
    @brief Saves the Actions of the given sequence table to the hdf file.

    Saved in a group called "actions". Each round has it's own dataset and a
    dataset called "index" stores references to each round's actions.
  */
  void SaveActions(const SequenceTable<kPlayers, kActions>& seq) {
    H5::Group group = CreateIndexedDSetGroup("actions");

    for (RoundId rid = 0; rid < kNRounds; ++rid) {
      Round round{rid};
      nda::const_vector_ref<AbstractAction> actions = seq.Actions(round);

      std::array<hsize_t, 1> round_fspace_dims = {actions.size()};
      H5::DataSpace round_fspace{1, round_fspace_dims.data()};
      H5::DataSet round_dataset = group.createDataSet(kRoundNames[rid].data(),
        AbstractActionType(), round_fspace);
      round_dataset.write(actions.data(), AbstractActionType());

      WriteRefToIndexDSet(group, round_dataset, rid);
    }
  }  // SaveActions()

  /*
    @brief Saves the given sequence table to the hdf file.

    Saved in a group called "sequences". Each round has it's own dataset and a
    dataset called "index" stores references to each round's sequences.
  */
  void SaveSequences(const SequenceTable<kPlayers, kActions>& seq) {
    H5::Group group = CreateIndexedDSetGroup("sequences");

    for (RoundId rid = 0; rid < kNRounds; ++rid) {
      Round round{rid};

      std::array<hsize_t, 2> round_fspace_dims = {seq.States(round),
                                                  seq.ActionCount(round)};
      H5::DataSpace round_fspace{2, round_fspace_dims.data()};
      H5::DataSet round_dataset = group.createDataSet(kRoundNames[rid].data(),
        H5::PredType::NATIVE_UINT32, round_fspace);
      round_fspace.selectAll();
      std::array<hsize_t, 1> round_mspace_dims = {seq.table()[rid].size()};
      H5::DataSpace round_mspace{1, round_mspace_dims.data()};
      round_mspace.selectAll();
      round_dataset.write(seq.table()[rid].data(), H5::PredType::NATIVE_UINT32,
                          round_mspace, round_fspace);

      WriteRefToIndexDSet(group, round_dataset, rid);
    }
  }  // SaveSequences()

  /*
    @brief Saves the given information abstraction's cluster mapping.

    Saved in a group called "clusters". Each round has it's own dataset and a
    dataset called "index" stores references to each round's clusters.
  */
  void SaveClusters(const InfoAbstraction& ia) {
    H5::Group group = CreateIndexedDSetGroup("clusters");

    for (RoundId rid = 0; rid < kNRounds; ++rid) {
      std::array<hsize_t, 1> round_fspace_dims = {ia.table()[rid].size()};
      H5::DataSpace round_fspace{1, round_fspace_dims.data()};
      H5::DataSet round_dataset = group.createDataSet(kRoundNames[rid].data(),
        H5::PredType::NATIVE_UINT32, round_fspace);
      round_fspace.selectAll();
      round_dataset.write(ia.table()[rid].data(), H5::PredType::NATIVE_UINT32,
                          round_fspace, round_fspace);

      WriteRefToIndexDSet(group, round_dataset, rid);
    }
  }  // SaveClusters()

  /*
    @brief Saves the given average's policy.

    Saved in a group called "policy". Each round has it's own dataset and a
    dataset called "index" stores references to each round's policy. All actions
    are included in each round, not just the legal ones, so each round's dataset
    is of size (clusters x sequences x actions).

    @param avg The average policy to save.
    @param verbose Whether to print progress information.
  */
  void SavePolicy(const typename Strategy<kPlayers, kActions, InfoAbstraction>::Average& avg,  // NOLINT
                  bool verbose = false) {
    H5::Group group = CreateIndexedDSetGroup("policy");

    for (RoundId rid = 0; rid < kNRounds; ++rid) {
      Round round{rid};

      const SequenceTable<kPlayers, kActions>& seq = avg.action_abstraction();
      const InfoAbstraction& ia = avg.info_abstraction();
      std::array<hsize_t, 3> round_fspace_dims = {ia.NumClusters(round),
                                                  seq.States(round),
                                                  seq.ActionCount(round)};
      H5::DataSpace round_fspace{3, round_fspace_dims.data()};
      H5::DataSet round_dataset = group.createDataSet(kRoundNames[rid].data(),
        H5::PredType::NATIVE_FLOAT, round_fspace);

      std::array<hsize_t, 1> m_space_dims = {round_fspace_dims[2]};
      H5::DataSpace m_space{1, m_space_dims.data()};
      m_space.selectAll();
      for (CardCluster cc = 0; cc < ia.NumClusters(round); ++cc) {
        for (SequenceId sid = 0; sid < seq.States(round); ++sid) {
          std::array<float, kActions> policy = avg.Policy(round, cc, sid);
          std::array<hsize_t, 3> select_count = {1, 1, round_fspace_dims[2]};
          std::array<hsize_t, 3> select_start = {cc, sid, 0};
          round_fspace.selectHyperslab(H5S_SELECT_SET, select_count.data(),
                                       select_start.data());
          round_dataset.write(policy.data(), H5::PredType::NATIVE_FLOAT,
                              m_space, round_fspace);
        }

        if (verbose) {
          std::cout << "wrote " << round << " cluster " << cc << std::endl;
        }
      }

      WriteRefToIndexDSet(group, round_dataset, rid);
    }
  }  // SavePolicy()

  /*
    @brief Saves an attribute to the root file.

    @param T The C++ type of the attribute.
    @param name The name of the attribute.
    @param dtype The h5 datatype of the attribute.
    @param to_save The value to write to the attribute.
  */
  template <typename T>
  void SaveAttribute(std::string_view name, typename H5::DataType dtype,
                     T to_save) {
    H5::DataSpace at_space;
    H5::Attribute at = average_.createAttribute(name.data(), dtype, at_space);
    at.write(dtype, &to_save);
  }  // SaveAttribute()

  /*
    @brief Checks that a root file attribute is what we expect it to be.

    Throws if it's not.

    @param T The C++ type of the attribute.
    @param name The name of the attribute to check.
    @param dtype The h5 datatype of the attribute.
    @param expected The expected value of the attribute.
  */
  template <typename T>
  void CheckAttribute(std::string_view name, typename H5::DataType dtype,
                      T expected) {
    H5::Attribute at = average_.openAttribute(name.data());
    T local_at;
    at.read(dtype, &local_at);
    if (local_at != expected) {
      std::stringstream error_ss;
      error_ss << "Attempted to open strategy file with attribute " << name
               << " of value " << +local_at << " but Scribe was expecting it "
               << "to have value" << +expected << std::endl;
      throw std::logic_error(error_ss.str());
    }
  }  // CheckAttribute()

 public:
  /*
    @brief Saves the given average as an hdf file and then uses that file.

    @param avg The average strategy to use.
    @param loc The location to save the hdf file.
    @param verbose Whether to print log information.
  */
  Scribe(const typename Strategy<kPlayers, kActions, InfoAbstraction>::Average& avg,  // NOLINT
         std::filesystem::path loc, bool verbose = false)
         : average_{loc.c_str(), H5F_ACC_EXCL} {
    SaveStartState(avg.action_abstraction().start_state());
    SaveActions(avg.action_abstraction());
    SaveSequences(avg.action_abstraction());
    SaveClusters(avg.info_abstraction());
    SavePolicy(avg, verbose);
    SaveAttribute<PlayerId>("kPlayers", H5::PredType::NATIVE_UINT8, kPlayers);
    SaveAttribute<uint64_t>("kActions", H5::PredType::NATIVE_UINT64, kActions);
  }

  /*
    @brief Loads the given hdf file and uses that file.
  */
  explicit Scribe(std::filesystem::path loc)
      : average_{loc.c_str(), H5F_ACC_RDONLY} {
    CheckAttribute<PlayerId>("kPlayers", H5::PredType::NATIVE_UINT8, kPlayers);
    CheckAttribute<uint64_t>("kActions", H5::PredType::NATIVE_UINT64, kActions);
  }

  /*
    @brief Returns the starting state of this strategy.
  */
  Node<kPlayers> StartState() {
    H5::DataSet start_state = average_.openDataSet("start_state");
    std::string start_state_json;
    std::size_t len = start_state.getDataType().getSize();
    H5::StrType mtype{H5::PredType::C_S1, len};
    start_state.read(start_state_json, mtype);
    std::stringstream ss_ss;
    ss_ss << start_state_json;
    Node<kPlayers> start_state_node;
    CerealLoadJSON(ss_ss, &start_state_node);
    return start_state_node;
  }  // StartState()

  /*
    @brief Returns the cluster of the given hand index in the given round.

    Meant for use by Matchmaker.
  */
  CardCluster GetCluster(Round round, hand_index_t idx) {
    H5::Group clusters = average_.openGroup("clusters");
    H5::DataSet round_clusters = clusters.openDataSet(
        kRoundNames[+round].data());
    H5::DataSpace fspace = round_clusters.getSpace();
    std::array<hsize_t, 1> selection_dim = {idx};
    fspace.selectElements(H5S_SELECT_SET, 1, selection_dim.data());
    H5::DataSpace mspace;
    CardCluster buffer;
    round_clusters.read(&buffer, H5::PredType::NATIVE_UINT32, mspace, fspace);
    return buffer;
  }  // GetCluster()

  /*
    @brief Returns an array of the strategy at the given game state.

    Values after this round's ActionCount() are undefined.

    Same as Strategy::Average's policy function.
  */
  std::array<float, kActions> Policy(Round round, CardCluster card_bucket,
                                     SequenceId seq) {
    H5::Group policy = average_.openGroup("policy");
    H5::DataSet round_policy = policy.openDataSet(kRoundNames[+round].data());
    H5::DataSpace fspace = round_policy.getSpace();
    std::array<hsize_t, 3> dataset_dims;
    fspace.getSimpleExtentDims(dataset_dims.data());
    std::array<hsize_t, 3> selection_size = {1, 1, dataset_dims[2]};
    std::array<hsize_t, 3> selection_start = {card_bucket, seq, 0};
    fspace.selectHyperslab(H5S_SELECT_SET, selection_size.data(),
                           selection_start.data());
    std::array<hsize_t, 1> mspace_dim = {dataset_dims[2]};
    H5::DataSpace mspace{1, mspace_dim.data()};
    std::array<float, kActions> policy_arr;
    round_policy.read(policy_arr.data(), H5::PredType::NATIVE_FLOAT, mspace,
                      fspace);
    return policy_arr;
  }  // Policy()

  /*
    @brief Returns the available actions at the given round.

    Values after this round's ActionCount() are undefined.

    Same as SequenceTable's Actions() function.
  */
  std::array<AbstractAction, kActions> Actions(Round round) {
    H5::Group actions = average_.openGroup("actions");
    H5::DataSet round_actions = actions.openDataSet(kRoundNames[+round].data());
    H5::DataSpace fspace = round_actions.getSpace();
    std::array<AbstractAction, kActions> action_arr;
    round_actions.read(action_arr.data(), AbstractActionType(), fspace,
                       fspace);
    return action_arr;
  }  // Actions()

  /*
    @brief Returns the number of available actions at the given round.

    Same as SequenceTable's ActionCount() function.
  */
  hsize_t ActionCount(Round round) {
    H5::Group actions = average_.openGroup("actions");
    H5::DataSet round_actions = actions.openDataSet(kRoundNames[+round].data());
    H5::DataSpace fspace = round_actions.getSpace();
    std::array<hsize_t, 1> dataset_dim;
    fspace.getSimpleExtentDims(dataset_dim.data());
    return dataset_dim[0];
  }  // ActionCount()

  /*
    @brief Returns the game sequence reached by taking the given action.

    Same as SequenceTable's Next() function.
  */
  SequenceId Next(Round round, SequenceId current_node, hsize_t action_idx) {
    H5::Group sequences = average_.openGroup("sequences");
    H5::DataSet round_sequences = sequences.openDataSet(
        kRoundNames[+round].data());
    H5::DataSpace fspace = round_sequences.getSpace();
    std::array<hsize_t, 2> selection_point = {current_node, action_idx};
    fspace.selectElements(H5S_SELECT_SET, 1, selection_point.data());
    H5::DataSpace mspace;
    SequenceId buffer;
    round_sequences.read(&buffer, H5::PredType::NATIVE_UINT32, mspace, fspace);
    return buffer;
  }  // Next()

  /* @brief Scribe serialize function */
  template<class Archive>
  void save(Archive& archive) const {
    archive(average_.getFileName());
  }

  /* @brief Scribe deserialize function */
  template <class Archive>
  static void load_and_construct(
    Archive& ar,
    cereal::construct<Scribe<kPlayers, kActions, InfoAbstraction>>& construct
  ) {
    std::string file_name;
    ar(file_name);
    std::filesystem::path file_path{file_name};
    construct(file_path);
  }
};  // class Scribe

}  // namespace fishbait

#endif  // AI_SRC_RELAY_SCRIBE_H_
