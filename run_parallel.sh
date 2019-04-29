#!/bin/bash
parallel -j32 --timeout 1800 --joblog parallel_log ./optimized/vc_solver {} '>' output/{.}.output ::: graphs/*
