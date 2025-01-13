import chess
import chess.engine
import berserk
import tempfile
import os
import subprocess
from pathlib import Path

TOKEN = "your_lichess_token_here"
ENGINE_PATH = "./BlueHerring"  # Path to your compiled engine

class ChessEngineWrapper:
    def __init__(self):
        self.engine_path = ENGINE_PATH
        
    def prepare_input_file(self, moves):
        # Create a temporary file for move history
        fd, history_path = tempfile.mkstemp(suffix='.txt')
        os.close(fd)
        
        # Create a temporary file for engine output
        fd, output_path = tempfile.mkstemp(suffix='.txt')
        os.close(fd)
        
        # Write moves to history file
        with open(history_path, 'w') as f:
            for move in moves:
                f.write(f"{move}\n")
                
        return history_path, output_path

    def get_engine_move(self, moves):
        # Prepare input/output files
        history_path, output_path = self.prepare_input_file(moves)
        
        try:
            # Call your engine with the correct arguments
            subprocess.run([
                self.engine_path,
                "-H", history_path,
                "-m", output_path
            ], check=True)
            
            # Read the engine's move from output file
            with open(output_path, 'r') as f:
                engine_move = f.read().strip()
            
            return engine_move
            
        finally:
            # Cleanup temporary files
            os.unlink(history_path)
            os.unlink(output_path)

def main():
    # Initialize Lichess connection
    session = berserk.TokenSession(TOKEN)
    client = berserk.Client(session)
    
    # Initialize engine wrapper
    engine_wrapper = ChessEngineWrapper()
    
    # Accept challenges and play games
    for event in client.bots.stream_incoming_events():
        if event['type'] == 'challenge':
            client.bots.accept_challenge(event['challenge']['id'])
            
        elif event['type'] == 'gameStart':
            game_id = event['game']['id']
            
            # Initialize game state
            board = chess.Board()
            moves_history = []
            
            # Stream the game state
            for state in client.bots.stream_game_state(game_id):
                if state['type'] == 'gameState':
                    # Convert UCI moves to your engine's format
                    moves = state['moves'].split()
                    
                    # Update moves history
                    if moves:
                        moves_history = [move for move in moves]
                    
                    # Check if it's our turn
                    is_our_turn = len(moves) % 2 == (0 if state['white']['id'] == client.account.get()['id'] else 1)
                    
                    if is_our_turn:
                        # Get move from engine
                        engine_move = engine_wrapper.get_engine_move(moves_history)
                        
                        # Make the move on Lichess
                        client.bots.make_move(game_id, engine_move)
                        
                        # Update local board
                        board.push(chess.Move.from_uci(engine_move))

if __name__ == "__main__":
    main()