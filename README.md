# WeGotYouCovered #
## Description ##
Framework for finding a minimum vertex cover.

Demian Hespe, Sebastian Lamm, Christian Schulz and Darren Strash.

```
@inproceedings{hespe2020wegotyoucovered,
  title={WeGotYouCovered: The Winning Solver from the PACE 2019 Challenge, Vertex Cover Track},
  author={Hespe, Demian and Lamm, Sebastian and Schulz, Christian and Strash, Darren},
  booktitle={2020 Proceedings of the SIAM Workshop on Combinatorial Scientific Computing},
  pages={1--11},
  year={2020},
  organization={SIAM}
}
```

## Usage ##
`vc_solver FILE [options]`.

### Options ###
This is a brief overview of the most important options.
For a full description, please take a look at the user guide.

`FILE`
Path to graph file that you want the maximum independent set for.

`--help`
Print help.

`--console_log`
Write the log to the console.

`--seed=<int>`
Seed to use for the random number generator.

`--time_limit=<double>`
Time limit until the algorithm terminates.

