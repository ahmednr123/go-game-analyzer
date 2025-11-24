# Go Game Analyzer

SDL tool to analyze go games. The main purpose of this tool is to self analyze and learn to play go for beginners (like me).

![Game](screenshots/main.png)

Todo:
- ~~Compute board state after addAction~~
- ~~Use board state to check if stone is already added~~
- ~~Add GoBoardRuleManager to validate move (if stone can be added to the position)~~
- ~~Remove action: RemoveStones and add CaptureStones { GoStone, GoStone[] }, to know which stone addition caused the capture~~
- Handle Ko situation (repeating board position is not allowed)
- Connect to KataGo and evaluate board position
- Use 'space' to get next best move (based on the turn) (using a number before 'space' will update board state to that many best moves)
- Add functionality to switch between multiple engine elo for finding the best move only
- Add a loader to wait for KataGo to respond
- Display error and stop rendering if any error is prompted
- Add multiple platform build solution in CMakeLists.txt
- Use Github workflows to generate multiple releases for different platforms (Windows, Linux and MacOS)
