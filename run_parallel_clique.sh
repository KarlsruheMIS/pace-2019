#!/bin/bash
parallel -j32 --timeout 7200 --joblog parallel_log ./scripts/runClique.sh {} '>' output/{.}.output ::: graphs/*
