# Copyright 2021 Marzuk Rashid

add_library(
  relay SHARED
  commander.cc
  commander.h
  )
target_link_libraries(relay blueprint clustering poker utils)

add_executable(fill_db.out fill_db.cc)
target_link_libraries(fill_db.out pqxx pq blueprint clustering poker utils)
set_target_properties(fill_db.out PROPERTIES COMPILE_OPTIONS "")

file(WRITE init.sql
"-- Copyright 2021 Marzuk Rashid

\\set ON_ERROR_STOP true

create role fishbait with login password '${DB_PASSWORD}';
grant fishbait to postgres;
create database fishbait with owner fishbait;
\\c fishbait;

-- game_state_id is:
--   ((sequences_in_round * card_cluster) + sequence_id) +
--   ((sequences_in_round * card_clusters_in_round) for each previous rounds)
create table blueprint (
  game_state_id bigint primary key,
  action_policy real[]
);
grant all on blueprint to fishbait;
")
