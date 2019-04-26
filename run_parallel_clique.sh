#!/bin/bash
parallel -j32 --timeout 7200 --joblog parallel_log ./optimized/kernelizeAndMakeCliqueInstance {} '>' output/{.}.output ::: graphs/*
