# The Monopoly of NTU (C++ Edition)
This is the final project for the course "Programming Design" at National Taiwan University .

### Brief Description
This project is developed using C++ and the SFML library. It provides users with a simple framework for a standalone version of a Monopoly-style game, allowing for customization based on their needs.

### Collaborators
Yi-Lun Hong, Hong-Kai Yang, En-Chia Chao

### File Description: 
1. `rich.cpp`: The main program file that ensures the game runs properly.
2. `1.png`, `2.png`, `3.png`, `4.png`, `5.png`, and `6.png`: Contains six PNG files of dice, enabling the game interface to display dice images. Users can replace these with their own versions.
3. `NotoSansTC-Regular.ttf`: Sample font file, used to properly display Traditional Chinese characters in the window. Users can replace it with other fonts.

### Execution Method
1. Please place `1.png`, `2.png`, `3.png`, `4.png`, `5.png`, and `6.png` into a folder named "dice".
2. Please place `NotoSansTC-Regular.ttf` into a folder named "fonts".
3. Place `rich.cpp`, `fonts` and the `dice` folder in the same directory.  
4. Compile using the command: "g++ -o rich rich.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system -std=c++11"
5. Run using the command: "./rich"
