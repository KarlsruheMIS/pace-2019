#!/bin/bash
parallel -j32 --timeout 7200 --joblog parallel_log ./optimized/vc_solver {} --time_limit=7200 --console_log '>' output/{.}.output ::: graphs/*
