/**
 * parse_parameters.h
 * Purpose: Parse command line parameters.
 *
 ******************************************************************************
 * Copyright (C) 2015-2017 Sebastian Lamm <lamm@ira.uka.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _PARSE_PARAMETERS_H_
#define _PARSE_PARAMETERS_H_

#include <omp.h>

#include "configuration_mis.h"

/**
 * Parse the given parameters and apply them to the config.
 *
 * @param argn Number of parameters.
 * @param argv Values of the parameters.
 * @param mis_config Config to store the values in.
 * @param graph_filename String to store the filename of the graph.
 *
 * @return -1 if there was an error. 0 otherwise.
 */
int parse_parameters(int argn, char **argv,
                     MISConfig & mis_config,
                     std::string & graph_filename) {
    const char *progname = argv[0];

    // Setup the argtable structs
    struct arg_lit *help                = arg_lit0(NULL, "help", "Print help.");
    struct arg_int *user_seed           = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");

    struct arg_str *filename            = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to graph file.");
    struct arg_dbl *time_limit          = arg_dbl0(NULL, "time_limit", NULL, "Time limit in s. Default 1000s.");
    struct arg_lit *console_log         = arg_lit0(NULL, "console_log", "Stream the log into the console");

    struct arg_end *end                 = arg_end(100);

    // Setup the argtable
    void *argtable[] = {
            help, 
            filename, 
            user_seed, 
            time_limit, 
            console_log,
            end
    };    

    // Choose standard configuration
    configuration_mis cfg;
    cfg.standard(mis_config);
    
    // Parse the arguments
    int nerrors = arg_parse(argn, argv, argtable);

    if (help->count > 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-40s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    if (nerrors > 0) {
        arg_print_errors(stderr, end, progname);
        printf("Try '%s --help' for more information.\n", progname);
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    if (filename->count > 0) {
        graph_filename = filename->sval[0];
    }   

    if (user_seed->count > 0) {
        mis_config.seed = user_seed->ival[0];
    }

    if (time_limit->count > 0) {
        mis_config.time_limit = time_limit->dval[0];
    }

    if (console_log->count > 0) {
        mis_config.console_log = true;
        mis_config.print_log = false;
    } else {
        mis_config.print_log = true;
    }

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

    return 0;
}

#endif
