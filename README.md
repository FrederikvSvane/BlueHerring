# BlueHerring: A Command Line Interface (CLI) Chess Bot

Written in C++ for the course CSC_2F001_EP: Object-oriented Programming in C++, at École Polytechnique


## Description

This is an informative description of the project that makes it seem very interesting


## Getting started

### Dependencies

None so far



### Installing

Clone this repository to your machine


### Executing program

Navigate to the directory and run the following commands:

```
./build.sh
```
```
./BlueHerring -H input.txt -m output_example.txt
```

To play a game using the referee program used for this project (that you can install here https://github.com/julien-tierny/chess-referee-bin), run the following command:

```
chessGame -W <path to a chess program> -B <path to another chess program>
```

For example :
```
chessGame -W ./BlueHerring -B chessRandomPlayer
```


# Authors

Charbonneau, Simon (simon.charbonneau@polytechnique.edu)

Krause, Antonia Gerlach (antonia.gerlach-krause@polytechnique.edu)

Losel (sherab-losel.matos@polytechnique.edu)

Svane, Frederik (frederik.svane@polytechnique.edu)


# Referencing

The lines in our book of openings was taken from this github repository (https://github.com/lichess-org/chess-openings), specifically the ```uci``` field from ```dist/```.