# Description

BlueHerring is not a Red Herring. It's a deterministic CLI chess engine that honors the methodologies of the fellow oceanic-themed chess bot Stockfish. 
It integrates nicely with the Lichess UI. Granted that you create your own Lichess bot api key.

## Current Elo

~2000

### Dependencies

None


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


# Referencing

The lines in our book of openings was taken from this github repository (https://github.com/lichess-org/chess-openings), specifically the ```uci``` field from ```dist/```.
