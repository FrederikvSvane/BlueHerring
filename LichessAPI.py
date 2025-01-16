import berserk
import tempfile
import os
import subprocess
import sys
import time
from requests.exceptions import ConnectionError
from berserk.exceptions import ApiError
import json

with open('config.json') as f:
    config = json.load(f)
TOKEN = config['lichess_token']

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
    while True:  # Main reconnection loop
        try:
            session = berserk.TokenSession(TOKEN)
            client = berserk.Client(session)
            engine = ChessEngineWrapper()
            
            print("Starting bot...")
            last_event_type = None
            
            for event in client.bots.stream_incoming_events():
                print(f"\nReceived event: {event['type']}")
                
                # Handle challenges (including rematches)
                if event['type'] == 'challenge':
                    # Auto accept if it's a rematch (previous event was gameFinish)
                    if last_event_type == 'gameFinish':
                        print("Accepting rematch!")
                    else:
                        print("Accepting challenge!")
                    client.bots.accept_challenge(event['challenge']['id'])
                
                # Update last event type
                last_event_type = event['type']
                
                if event['type'] == 'gameStart':
                    game_id = event['game']['id']
                    bot_is_white = event['game']['color'] == 'white'
                    print(f"Game started! ID: {game_id}, Playing as: {'white' if bot_is_white else 'black'}")
                    
                    try:
                        for state in client.bots.stream_game_state(game_id):
                            if state['type'] == 'gameFull':
                                moves = state['state']['moves'].split() if state['state']['moves'] else []
                            elif state['type'] == 'gameState':
                                moves = state['moves'].split() if state['moves'] else []
                            
                            is_our_turn = len(moves) % 2 == (0 if bot_is_white else 1)
                            print(f"Moves: {moves}")
                            print(f"Our turn: {is_our_turn}")
                            
                            if is_our_turn:
                                print("Getting engine move...")
                                engine_move = engine.get_engine_move(moves)
                                print(f"Engine suggests: {engine_move}")
                                
                                # Retry logic for making moves
                                max_retries = 3
                                for attempt in range(max_retries):
                                    try:
                                        client.bots.make_move(game_id, engine_move)
                                        print("Move made!")
                                        break
                                    except (ConnectionError, ApiError) as e:
                                        if attempt == max_retries - 1:
                                            raise
                                        print(f"Move failed, retrying... (attempt {attempt + 1}/{max_retries})")
                                        time.sleep(1)
                                
                    except (ConnectionError, ApiError) as e:
                        print(f"Game {game_id} ended or connection lost: {e}")
                        continue
                        
        except KeyboardInterrupt:
            print("\nStopping bot...")
            sys.exit()
            
        except Exception as e:
            print(f"Error occurred: {e}")
            print("Reconnecting in 5 seconds...")
            time.sleep(5)

if __name__ == "__main__":
    main()