# Go Game Analyzer

SDL tool to analyze go games. The main purpose of this tool is to self analyze and learn to play go for beginners (like me).

![Game](screenshots/main.png)

Todo:
- ~~Compute board state after addAction~~
- ~~Use board state to check if stone is already added~~
- ~~Add GoBoardRuleManager to validate move (if stone can be added to the position)~~
- ~~Remove action: RemoveStones and add CaptureStones { GoStone, GoStone[] }, to know which stone addition caused the capture~~
- ~~Add undo and redo functionality using 'u' and 'r' key~~
- ~~Store captures info~~
- ~~Add functionality to pass for both turns (End game if both pass)~~
- ~~Display turn pass in UI~~
- ~~Connect to KataGo and evaluate board position~~
- ~~Use 'space' to get next best move (based on the turn)~~
- ~~Add multiple platform build solution in CMakeLists.txt~~
- ~~Use Github workflows to generate multiple releases for different platforms (Windows, Linux and MacOS)~~
- ~~Evaluate score (using katogo) and display it (ownership) (maybe)~~
- ~~Add functionality to switch between multiple engine elo for finding the best move only~~
- ~~Add a loader to wait for KataGo to respond~~
- ~~View ownership (Update ownership 2d array on every move) (Very slow! so use something to handle new requests if waiting for old ones)~~
- ~~show engine is busy and stop critical actions (addStone & pass)~~
- ~~Show score and final winner~~
- ~~Fix addStone & pass duplicate code (use variant?)~~
- ~~BUG: Use threads for AI responses (callback hangs rendering)~~
- ~~Add sound effects & music~~
- ~~Windows code to handle KataGo communication~~
- ~~Display error and stop rendering if any error is prompted~~
- ~~Add theme functionality~~
- Handle Ko situation (repeating board position is not allowed)
- Let AI play from the current turn (Switch to "moves" & "initialMoves")
- Bundle with KataGo

Next Release:
- Human like play using b18c384nbt-humanv0.bin.gz model
- Saving and loading game states
- Finding dead groups and show moves that lead to group death (promising: lifego_b18.bin.gz fine-tuned model)
- Find mistakes and point whats the aftermath
