#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <sstream>

// Constants
const int NUM_TILES = 36; // Total number of tiles on the map
const int gridSize = 10;

// Mapping from names to price and toll (can be adjusted as needed)
std::map<std::wstring, std::pair<int, int>> NamesToPrice = {
    {L"小傑辦公室", {850, 625}}, {L"管一", {700, 500}}, {L"管二", {600, 350}},
    {L"禮賢樓", {500, 300}}, {L"管圖", {350, 200}}, {L"教研", {400, 260}},
    {L"總圖", {750, 550}}, {L"社科圖", {675, 450}}, {L"普通", {572, 374}},
    {L"博雅", {460, 278}}, {L"新生", {380, 262}}, {L"共同", {300, 175}},
    {L"小福", {512, 306}}, {L"小小福", {375, 258}}, {L"女九餐廳", {320, 180}},
    {L"活大", {685, 482}}, {L"118巷", {590, 340}}, {L"小木屋鬆餅", {770, 570}},
    {L"醉月湖", {608, 354}}, {L"新體", {342, 190}}, {L"水源BOT", {800, 575}},
    {L"傅鐘", {405, 265}}, {L"大考中心", {490, 290}}, {L"捷運公館站", {715, 515}}
    // Note: "Chance" and "Fate" tiles do not need price and toll
};

// Chance cards
std::vector<std::pair<std::wstring, int>> chanceCards = {
    {L"跟小傑老師打高爾夫球後去吃熱炒，積分增加800分", 800},
    {L"期中考偷看別人且沒被抓到 玩家的積分500分", 500},
    {L"期中報告隊友超強 躺分躺得恰到好處 積分加600分", 600},
    {L"55688發計程車折價券!!! 玩家積分增加400分", 400},
    {L"管二大廳整修完畢 旋轉樓梯讓管院大樓高級感加倍 玩家積分加500", 500},
    {L"雖然玩家平常課業繁重 但該玩的還是不能少 玩家揪三五好友去台大音樂節放鬆充電 度過美好的一週!積分加700", 700},
    {L"小木屋鬆餅買一送一 玩家花十分鐘排隊買了藍莓鬆餅 心情超滿足，加300分", 300},
    {L"學生證弄丟第七次:( 花費200點積分得到新的學生證!!", -200},
    {L"學生證弄丟第18次 但有順利找回來 加500點積分", 500},
    {L"520看到小傑在跟多慧約會 心情大受打擊，一整天上課都提不起勁 積分扣100分", -100},
    {L"在醉月湖餵食禽鳥 被工友伯伯看到被訓斥一頓 上課還遲到，扣150分", -150},
    {L"你忘記了重要的報告截止日期 被組員白眼，扣100分", -100},
    {L"在總圖讀書打瞌睡 錯過開會的時間，扣50分", -50},
    {L"你為了參加台大杜鵑花節與音樂節 忘記完成作業，罰款 $300", -300},
    {L"你在小福買牛奶的期間 水源阿北把你的腳踏車拖到水源拖吊場 玩家氣到不行!!!! 積分扣200", -200},
    {L"教育部通過以後體恤老師與行政人員 決定教師節與勞動節大專院校皆放假 玩家的積分加500分", 500},
    {L"你參加聚會過多 學業受到影響，罰款 $50", -50},
    {L"你在宿舍因為一時貪玩 弄壞了公共設施，賠償 $200", -200},
    {L"寫實習履歷熬夜爆肝 但獲得投顧暑假實習 加300分", 300},
    {L"你的作業遲交 又因為受夠台北陰晴不定的天氣錯過公車 遲進教室，被微積分老師狠狠記住， 扣200分", -200},
    {L"該玩家參加體育課時受傷 意外被漂亮的校花帶去保健室 雖然受傷但內心超快樂，加500分", 500},
    {L"學會邊騎腳踏車邊撐傘 上下學超方便，加300分", 300},
    {L"週三施工不能走舟山路，繞道而行 積分減100", -100},
    {L"台大以後聖誕節與平安夜皆放假，積分加200分", 200}
};

// wrapText function for handling Chinese text auto-wrap
std::wstring wrapText(const std::wstring& string, unsigned int maxWidth, sf::Font& font, unsigned int characterSize) {
    std::wstring result;
    std::wstring line;
    for (size_t i = 0; i < string.length(); ++i) {
        line += string[i];
        sf::Text text(line, font, characterSize);
        if (text.getLocalBounds().width > maxWidth) {
            // Remove the last character
            line.erase(line.end() - 1);
            result += line + L"\n";
            // Start new line with the current character
            line = string[i];
        }
    }
    result += line;
    return result;
}

struct Tile
{
    int index;                   // Tile index
    std::wstring name;           // Tile name
    sf::RectangleShape shape;    // Tile rectangle
    sf::Text text;               // Text displayed on the tile
    sf::Color color;             // Tile color
    int price;                   // Tile price
    int toll;                    // Tile toll
    int owner;                   // Tile owner, -1 means no owner

    Tile(int index, const std::wstring& name, const sf::Vector2f& size, const sf::Color& color, sf::Font& font, int price = 0, int toll = 0)
        : index(index), name(name), shape(size), color(color), price(price), toll(toll), owner(-1) {
        shape.setFillColor(color);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);

        // Initialize the tile name text with word-wrapping
        text.setFont(font);
        unsigned int maxWidth = static_cast<unsigned int>(size.x - 10); // leave some margin
        text.setString(wrapText(name, maxWidth, font, 14));
        text.setCharacterSize(14);
        text.setFillColor(sf::Color::Black);

        // Center the text on the tile
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                       textBounds.top + textBounds.height / 2.0f);
    }

    void setPosition(float x, float y) {
        shape.setPosition(x, y);
        // Center the text on the tile
        text.setPosition(
            x + shape.getSize().x / 2.0f,
            y + shape.getSize().y / 2.0f - 10 // adjust vertical offset
        );
    }
};

struct Player
{
    std::wstring name;                        // Player name
    int money;                                // Player money
    int position;                             // Player current tile position
    sf::Color color;                          // Player color
    std::vector<std::wstring> properties;     // Player owned properties
    bool skipTurn;                            // Whether to skip the next turn

    Player(const std::wstring& name, int money, sf::Color color)
        : name(name), money(money), position(0), color(color), skipTurn(false) {}
};

// Function to calculate tile position
sf::Vector2f calculateTilePosition(int index, float tileSize, int gridSize)
{
    int totalTilesPerSide = gridSize - 1; // number of tiles on each side
    int maxIndex = totalTilesPerSide * 4; // total number of tiles on the board perimeter

    if (index >= 0 && index < totalTilesPerSide) {
        // Top row, left to right
        return sf::Vector2f(index * tileSize, 0);
    }
    else if (index >= totalTilesPerSide && index < totalTilesPerSide * 2) {
        // Right column, top to bottom
        int idx = index - totalTilesPerSide;
        return sf::Vector2f((gridSize - 1) * tileSize, idx * tileSize);
    }
    else if (index >= totalTilesPerSide * 2 && index < totalTilesPerSide * 3)
    {
        // Bottom row, right to left
        int idx = index - totalTilesPerSide * 2;
        return sf::Vector2f((gridSize - 1 - idx) * tileSize, (gridSize - 1) * tileSize);
    }
    else if (index >= totalTilesPerSide * 3 && index < maxIndex) {
        // Left column, bottom to top
        int idx = index - totalTilesPerSide * 3;
        return sf::Vector2f(0, (gridSize - 1 - idx) * tileSize);
    }
    else {
        // Index out of range
        return sf::Vector2f(0, 0);
    }
}

// Enum for game state
enum class GameState
{
    PlayerNumberSelection,
    InGame,
    GameOver
};

int main()
{
    int effect = 0; // Used to store event impact (e.g., money increase/decrease)
    bool isGameOver = false;
    sf::RenderWindow window(sf::VideoMode(800, 800), L"ntu_monopoly");
    const float tileSize = 80.0f; // Size of each tile
    std::vector<sf::RectangleShape> ownerMarks;

    // Load a font that supports Traditional Chinese (e.g., Noto Sans TC)
    sf::Font font;
    if (!font.loadFromFile("fonts/NotoSansTC-Regular.ttf")) { // Ensure the font path is correct
        std::wcout << L"Failed to load font! Make sure 'NotoSansTC-Regular.ttf' exists." << std::endl;
        return -1;
    }

    // Define game state
    GameState gameState = GameState::PlayerNumberSelection;

    int numberOfPlayers = 0; // Number of players

    // Player number selection buttons
    sf::RectangleShape twoPlayersButton(sf::Vector2f(200, 50));
    twoPlayersButton.setFillColor(sf::Color::White);
    twoPlayersButton.setOutlineThickness(2);
    twoPlayersButton.setOutlineColor(sf::Color::Black);
    twoPlayersButton.setPosition(300, 250);

    sf::RectangleShape threePlayersButton(sf::Vector2f(200, 50));
    threePlayersButton.setFillColor(sf::Color::White);
    threePlayersButton.setOutlineThickness(2);
    threePlayersButton.setOutlineColor(sf::Color::Black);
    threePlayersButton.setPosition(300, 350);

    sf::RectangleShape fourPlayersButton(sf::Vector2f(200, 50));
    fourPlayersButton.setFillColor(sf::Color::White);
    fourPlayersButton.setOutlineThickness(2);
    fourPlayersButton.setOutlineColor(sf::Color::Black);
    fourPlayersButton.setPosition(300, 450);

    // Base and hover colors for player number buttons
    sf::Color playerButtonBaseColor = sf::Color::White;
    sf::Color playerButtonHoverColor = sf::Color(200, 200, 200); // Light gray

    // Button texts
    sf::Text twoPlayersButtonText;
    twoPlayersButtonText.setFont(font);
    twoPlayersButtonText.setString(L"2 位玩家");
    twoPlayersButtonText.setCharacterSize(20);
    twoPlayersButtonText.setFillColor(sf::Color::Black);
    sf::FloatRect twoPlayersTextBounds = twoPlayersButtonText.getLocalBounds();
    twoPlayersButtonText.setOrigin(twoPlayersTextBounds.left + twoPlayersTextBounds.width / 2.0f,
                                   twoPlayersTextBounds.top + twoPlayersTextBounds.height / 2.0f);
    twoPlayersButtonText.setPosition(
        twoPlayersButton.getPosition().x + twoPlayersButton.getSize().x / 2.0f,
        twoPlayersButton.getPosition().y + twoPlayersButton.getSize().y / 2.0f - 5);

    sf::Text threePlayersButtonText;
    threePlayersButtonText.setFont(font);
    threePlayersButtonText.setString(L"3 位玩家");
    threePlayersButtonText.setCharacterSize(20);
    threePlayersButtonText.setFillColor(sf::Color::Black);
    sf::FloatRect threePlayersTextBounds = threePlayersButtonText.getLocalBounds();
    threePlayersButtonText.setOrigin(threePlayersTextBounds.left + threePlayersTextBounds.width / 2.0f,
                                     threePlayersTextBounds.top + threePlayersTextBounds.height / 2.0f);
    threePlayersButtonText.setPosition(
        threePlayersButton.getPosition().x + threePlayersButton.getSize().x / 2.0f,
        threePlayersButton.getPosition().y + threePlayersButton.getSize().y / 2.0f - 5);

    sf::Text fourPlayersButtonText;
    fourPlayersButtonText.setFont(font);
    fourPlayersButtonText.setString(L"4 位玩家");
    fourPlayersButtonText.setCharacterSize(20);
    fourPlayersButtonText.setFillColor(sf::Color::Black);
    sf::FloatRect fourPlayersTextBounds = fourPlayersButtonText.getLocalBounds();
    fourPlayersButtonText.setOrigin(fourPlayersTextBounds.left + fourPlayersTextBounds.width / 2.0f,
                                    fourPlayersTextBounds.top + fourPlayersTextBounds.height / 2.0f);
    fourPlayersButtonText.setPosition(
        fourPlayersButton.getPosition().x + fourPlayersButton.getSize().x / 2.0f,
        fourPlayersButton.getPosition().y + fourPlayersButton.getSize().y / 2.0f - 5);

    // Add title text
    sf::Text selectPlayerTitle;
    selectPlayerTitle.setFont(font);
    selectPlayerTitle.setString(L"請選擇玩家人數");
    selectPlayerTitle.setCharacterSize(30);
    selectPlayerTitle.setFillColor(sf::Color::Black);
    sf::FloatRect titleBounds = selectPlayerTitle.getLocalBounds();
    selectPlayerTitle.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                                titleBounds.top + titleBounds.height / 2.0f);
    selectPlayerTitle.setPosition(
        window.getSize().x / 2.0f,
        150);

    // Roll dice button
    sf::RectangleShape button(sf::Vector2f(200, 50));
    button.setFillColor(sf::Color::White);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::Black);
    button.setPosition(300, 140);

    // Base and hover colors for the dice roll button
    sf::Color buttonBaseColor = sf::Color::White;
    sf::Color buttonHoverColor = sf::Color(200, 200, 200); // Light gray

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(L"擲骰子"); // Dice roll in Traditional Chinese
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::Black);
    // Center text on the button
    sf::FloatRect buttonTextBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(buttonTextBounds.left + buttonTextBounds.width / 2.0f,
                        buttonTextBounds.top + buttonTextBounds.height / 2.0f);
    buttonText.setPosition(
        button.getPosition().x + button.getSize().x / 2.0f,
        button.getPosition().y + button.getSize().y / 2.0f - 5
    );

    // Confirm button
    sf::RectangleShape confirmButton(sf::Vector2f(100, 40));
    confirmButton.setFillColor(sf::Color::Green);
    confirmButton.setOutlineThickness(2);
    confirmButton.setOutlineColor(sf::Color::Black);
    confirmButton.setPosition(350, 420);

    // Base and hover colors for confirm button
    sf::Color confirmButtonBaseColor = sf::Color::Green;
    sf::Color confirmButtonHoverColor = sf::Color(100, 255, 100); // Light green

    sf::Text confirmButtonText;
    confirmButtonText.setFont(font);
    confirmButtonText.setCharacterSize(14);
    confirmButtonText.setFillColor(sf::Color::Black);
    confirmButtonText.setPosition(375, 430);
    confirmButtonText.setString(L"確認");

    // Yes button
    sf::RectangleShape yesButton(sf::Vector2f(100, 40));
    yesButton.setFillColor(sf::Color::Green);
    yesButton.setOutlineThickness(2);
    yesButton.setOutlineColor(sf::Color::Black);
    yesButton.setPosition(250, 420);

    // Base and hover colors for yes button
    sf::Color yesButtonBaseColor = sf::Color::Green;
    sf::Color yesButtonHoverColor = sf::Color(100, 255, 100); // Light green

    sf::Text yesButtonText;
    yesButtonText.setFont(font);
    yesButtonText.setCharacterSize(14);
    yesButtonText.setFillColor(sf::Color::Black);
    yesButtonText.setPosition(285, 430);
    yesButtonText.setString(L"是");

    // No button
    sf::RectangleShape noButton(sf::Vector2f(100, 40));
    noButton.setFillColor(sf::Color::Red);
    noButton.setOutlineThickness(2);
    noButton.setOutlineColor(sf::Color::Black);
    noButton.setPosition(450, 420);

    // Base and hover colors for no button
    sf::Color noButtonBaseColor = sf::Color::Red;
    sf::Color noButtonHoverColor = sf::Color(255, 100, 100); // Light red

    sf::Text noButtonText;
    noButtonText.setFont(font);
    noButtonText.setCharacterSize(14);
    noButtonText.setFillColor(sf::Color::Black);
    noButtonText.setPosition(485, 430);
    noButtonText.setString(L"否");

    // Exit button (pre-defined for hover)
    sf::RectangleShape exitButton(sf::Vector2f(100, 40));
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setOutlineThickness(2);
    exitButton.setOutlineColor(sf::Color::Black);
    exitButton.setPosition(350, 420);

    // Base and hover colors for exit button
    sf::Color exitButtonBaseColor = sf::Color::Red;
    sf::Color exitButtonHoverColor = sf::Color(255, 100, 100); // Light red

    sf::Text exitButtonText;
    exitButtonText.setFont(font);
    exitButtonText.setCharacterSize(14);
    exitButtonText.setFillColor(sf::Color::Black);
    exitButtonText.setPosition(375, 430);
    exitButtonText.setString(L"退出");

    // Dice display
    sf::Texture diceTextures[6];
    sf::Sprite diceSprite;
    diceSprite.setScale(0.5f, 0.5f);
    for (int i = 0; i < 6; ++i) {
        std::wstring dicePath = L"dice/" + std::to_wstring(i + 1) + L".png"; // assume dice images are in 'dice' folder
        std::string dicePathStr(dicePath.begin(), dicePath.end());
        if (!diceTextures[i].loadFromFile(dicePathStr)) {
            std::wcout << L"Cannot load dice image " << i + 1 << L".png" << std::endl;
            return -1;
        }
    }

    diceSprite.setTexture(diceTextures[0]); // default dice image
    diceSprite.setPosition(350, 280);       // position of the dice image

    // Initialize random seed
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Initialize tiles
    std::vector<Tile> tiles;

    // Corner tile names (four corners)
    std::vector<std::wstring> cornerNames = {L"起點", L"YouBike 站 1", L"監獄", L"YouBike 站 2"};

    // Special tile names (Chance and Fate), total 8
    std::vector<std::wstring> specialNames = {L"機會", L"命運", L"機會", L"命運", L"機會", L"命運", L"機會", L"命運"};

    // Normal tile names, total 24
    std::vector<std::wstring> normalNames = {
        L"小傑辦公室", L"管一", L"管二", L"禮賢樓", L"管圖", L"教研",
        L"總圖", L"社科圖", L"普通", L"博雅", L"新生", L"共同",
        L"小福", L"小小福", L"女九餐廳", L"活大", L"118巷", L"小木屋鬆餅",
        L"醉月湖", L"新體", L"水源BOT", L"傅鐘", L"大考中心", L"捷運公館站"};

    int normalNameIndex = 0;
    int specialNameIndex = 0;

    for (int index = 0; index < NUM_TILES; ++index) {
        std::wstring tileName;
        sf::Color tileColor;
        int price = 0, toll = 0;

        if (index == 0) {
            // Start
            tileName = cornerNames[0];
            tileColor = sf::Color::Cyan;
        }
        else if (index == 9) {
            // YouBike Station 1
            tileName = cornerNames[1];
            tileColor = sf::Color::Cyan;
        }
        else if (index == 18) {
            // Jail
            tileName = cornerNames[2];
            tileColor = sf::Color::Cyan;
        }
        else if (index == 27) {
            // YouBike Station 2
            tileName = cornerNames[3];
            tileColor = sf::Color::Cyan;
        }
        else if (index == 3 || index == 6 || index == 12 || index == 15 || index == 21 || index == 24 || index == 30 || index == 33) {
            // Chance and Fate
            tileName = specialNames[specialNameIndex++];
            tileColor = sf::Color(255, 165, 0);
        }
        else {
            // Normal tiles
            if (normalNameIndex < normalNames.size()) {
                tileName = normalNames[normalNameIndex++];
                tileColor = sf::Color::Yellow;
                auto it = NamesToPrice.find(tileName);
                if (it != NamesToPrice.end()) {
                    price = it->second.first;
                    toll = it->second.second;
                } else {
                    std::wcout << L"Warning: Price info not found for " << tileName << std::endl;
                }
            } else {
                std::wcout << L"Error: Not enough properties." << std::endl;
            }
        }

        // Initialize Tile
        Tile tile(index, tileName, sf::Vector2f(tileSize, tileSize), tileColor, font, price, toll);
        sf::Vector2f position = calculateTilePosition(index, tileSize, gridSize);
        tile.setPosition(position.x, position.y);
        tiles.push_back(tile);
    }

    // Ensure total tiles count matches NUM_TILES
    if (tiles.size() != NUM_TILES) {
        std::wcout << L"Tile initialization error. Expected " << NUM_TILES << L" tiles, got " << tiles.size() << std::endl;
        return -1;
    }

    // Define player-related variables
    std::vector<Player> players;
    std::vector<sf::CircleShape> playerCircles;
    std::vector<sf::Text> playerMoneyTexts;
    int currentPlayerIndex = 0; // Current player's index

    // Current turn text
    sf::Text currentPlayerText;
    currentPlayerText.setFont(font);
    currentPlayerText.setCharacterSize(20);
    currentPlayerText.setFillColor(sf::Color::Black);
    currentPlayerText.setPosition(300, 410);

    // Popup box background
    sf::RectangleShape popupBox(sf::Vector2f(400, 200));
    popupBox.setFillColor(sf::Color::White);
    popupBox.setOutlineThickness(2);
    popupBox.setOutlineColor(sf::Color::Black);
    popupBox.setPosition(200, 300); // center display

    // Popup text
    sf::Text popupText;
    popupText.setFont(font);
    popupText.setCharacterSize(16);
    popupText.setFillColor(sf::Color::Black);
    // Define maximum width for the popup text
    const unsigned int popupTextMaxWidth = static_cast<unsigned int>(popupBox.getSize().x - 40); // margin on each side
    // Adjust popupText position
    popupText.setPosition(
        popupBox.getPosition().x + 20, // left margin
        popupBox.getPosition().y + 20  // top margin
    );
    popupText.setString(L""); // Initially empty

    // Flags for popup
    bool isPopupActive = false;
    bool needToSwitchPlayer = false; // Do we need to switch to the next player?

    // Flag for purchase choice
    bool isChoiceActive = false; // Waiting for player to choose to buy property

    int tileToBuy = -1; // Index of the tile to buy

    // Dice roll animation variables
    bool isRollingDiceAnimation = false;
    sf::Clock diceAnimationClock;
    float diceAnimationDuration = 1.0f; // dice animation duration in seconds
    float diceAnimationElapsed = 0.0f;
    float diceImageChangeInterval = 0.1f; // change dice image every 0.1s
    float timeSinceLastDiceImageChange = 0.0f;

    sf::Clock deltaClock; // For calculating per-frame time

    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (gameState == GameState::PlayerNumberSelection)
            {
                // Handle events in the player number selection screen
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                        if (twoPlayersButton.getGlobalBounds().contains(mousePosF))
                        {
                            numberOfPlayers = 2;
                            // Initialize players
                            std::vector<sf::Color> playerColors = { sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Magenta };

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                Player player(L"玩家 " + std::to_wstring(i + 1), 1500, playerColors[i]);
                                players.push_back(player);
                            }

                            // Initialize player circles
                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                sf::CircleShape circle(10); // radius 10
                                circle.setFillColor(players[i].color);
                                circle.setOutlineThickness(2);
                                circle.setOutlineColor(sf::Color::Black);

                                // Position at the start tile with offsets
                                sf::Vector2f startPos = calculateTilePosition(players[i].position, tileSize, gridSize);
                                circle.setPosition(startPos.x + 10 + i * 15, startPos.y + 10 + i * 15);
                                playerCircles.push_back(circle);
                            }

                            // Initialize player money texts
                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                sf::Text moneyText;
                                moneyText.setFont(font);
                                moneyText.setCharacterSize(18);
                                moneyText.setFillColor(players[i].color);
                                moneyText.setPosition(300, 440 + i * 30);
                                moneyText.setString(players[i].name + L": $" + std::to_wstring(players[i].money));
                                playerMoneyTexts.push_back(moneyText);
                            }

                            currentPlayerText.setString(L"現在是 " + players[currentPlayerIndex].name + L" 的回合");

                            gameState = GameState::InGame;
                        }
                        else if (threePlayersButton.getGlobalBounds().contains(mousePosF))
                        {
                            numberOfPlayers = 3;
                            // Initialize players (same as above)
                            std::vector<sf::Color> playerColors = { sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Magenta };

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                Player player(L"玩家 " + std::to_wstring(i + 1), 1500, playerColors[i]);
                                players.push_back(player);
                            }

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                sf::CircleShape circle(10);
                                circle.setFillColor(players[i].color);
                                circle.setOutlineThickness(2);
                                circle.setOutlineColor(sf::Color::Black);

                                sf::Vector2f startPos = calculateTilePosition(players[i].position, tileSize, gridSize);
                                circle.setPosition(startPos.x + 10 + i * 15, startPos.y + 10 + i * 15);
                                playerCircles.push_back(circle);
                            }

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                sf::Text moneyText;
                                moneyText.setFont(font);
                                moneyText.setCharacterSize(18);
                                moneyText.setFillColor(players[i].color);
                                moneyText.setPosition(300, 440 + i * 30);
                                moneyText.setString(players[i].name + L": $" + std::to_wstring(players[i].money));
                                playerMoneyTexts.push_back(moneyText);
                            }

                            currentPlayerText.setString(L"現在是 " + players[currentPlayerIndex].name + L" 的回合");

                            gameState = GameState::InGame;
                        }
                        else if (fourPlayersButton.getGlobalBounds().contains(mousePosF))
                        {
                            numberOfPlayers = 4;
                            // Initialize players (same as above)
                            std::vector<sf::Color> playerColors = { sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Magenta };

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                Player player(L"玩家 " + std::to_wstring(i + 1), 1500, playerColors[i]);
                                players.push_back(player);
                            }

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                sf::CircleShape circle(10);
                                circle.setFillColor(players[i].color);
                                circle.setOutlineThickness(2);
                                circle.setOutlineColor(sf::Color::Black);

                                sf::Vector2f startPos = calculateTilePosition(players[i].position, tileSize, gridSize);
                                circle.setPosition(startPos.x + 10 + i * 15, startPos.y + 10 + i * 15);
                                playerCircles.push_back(circle);
                            }

                            for (int i = 0; i < numberOfPlayers; ++i)
                            {
                                sf::Text moneyText;
                                moneyText.setFont(font);
                                moneyText.setCharacterSize(18);
                                moneyText.setFillColor(players[i].color);
                                moneyText.setPosition(300, 440 + i * 30);
                                moneyText.setString(players[i].name + L": $" + std::to_wstring(players[i].money));
                                playerMoneyTexts.push_back(moneyText);
                            }

                            currentPlayerText.setString(L"現在是 " + players[currentPlayerIndex].name + L" 的回合");

                            gameState = GameState::InGame;
                        }
                    }
                }
            }
            else if (gameState == GameState::InGame)
            {
                if (isPopupActive)
                {
                    // Handle events inside the popup
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                            if (isChoiceActive)
                            {
                                if (yesButton.getGlobalBounds().contains(mousePosF))
                                {
                                    // Player chooses to buy
                                    Tile& tile = tiles[tileToBuy];
                                    players[currentPlayerIndex].money -= tile.price;
                                    tile.owner = currentPlayerIndex;
                                    players[currentPlayerIndex].properties.push_back(tile.name);

                                    // Draw a small rectangle at the bottom-right corner of the tile to indicate ownership
                                    sf::RectangleShape ownerMark(sf::Vector2f(10, 10));
                                    ownerMark.setFillColor(players[currentPlayerIndex].color);
                                    ownerMark.setPosition(
                                        tile.shape.getPosition().x + tileSize - 12,
                                        tile.shape.getPosition().y + tileSize - 12);
                                    ownerMarks.push_back(ownerMark);

                                    // Update player money text
                                    playerMoneyTexts[currentPlayerIndex].setString(
                                        players[currentPlayerIndex].name + L": $" + std::to_wstring(players[currentPlayerIndex].money));

                                    isPopupActive = false;
                                    isChoiceActive = false;

                                    // Need to switch player
                                    needToSwitchPlayer = true;
                                }
                                else if (noButton.getGlobalBounds().contains(mousePosF))
                                {
                                    // Player chooses not to buy
                                    isPopupActive = false;
                                    isChoiceActive = false;

                                    // Need to switch player
                                    needToSwitchPlayer = true;
                                }
                            }
                            else
                            {
                                if (isGameOver) {
                                    // Handle exit button click
                                    if (exitButton.getGlobalBounds().contains(mousePosF))
                                    {
                                        window.close();
                                    }
                                }
                                else {
                                    if (confirmButton.getGlobalBounds().contains(mousePosF))
                                    {
                                        // Close the popup
                                        isPopupActive = false;

                                        // Switch player if needed
                                        if (needToSwitchPlayer)
                                        {
                                            needToSwitchPlayer = false;
                                            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
                                            currentPlayerText.setString(L"現在是 " + players[currentPlayerIndex].name + L" 的回合");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (!isRollingDiceAnimation)
                {
                    // Handle in-game events
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                            if (button.getGlobalBounds().contains(mousePosF))
                            {
                                if (!isRollingDiceAnimation && !isPopupActive && !isChoiceActive)
                                {
                                    // Check if player needs to skip turn
                                    if (players[currentPlayerIndex].skipTurn)
                                    {
                                        players[currentPlayerIndex].skipTurn = false; // reset skip status
                                        popupText.setString(wrapText(players[currentPlayerIndex].name + L" 在監獄中，跳過一回合。", popupTextMaxWidth, font, 16));
                                        isPopupActive = true;
                                        needToSwitchPlayer = true;
                                    }
                                    else
                                    {
                                        // Start dice roll animation
                                        isRollingDiceAnimation = true;
                                        diceAnimationElapsed = 0.0f;
                                        timeSinceLastDiceImageChange = 0.0f;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Mouse position for hover effects
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if (gameState == GameState::PlayerNumberSelection)
        {
            // Handle hover effects for player number buttons
            if (twoPlayersButton.getGlobalBounds().contains(mousePosF)) {
                twoPlayersButton.setFillColor(playerButtonHoverColor);
            }
            else {
                twoPlayersButton.setFillColor(playerButtonBaseColor);
            }

            if (threePlayersButton.getGlobalBounds().contains(mousePosF)) {
                threePlayersButton.setFillColor(playerButtonHoverColor);
            }
            else {
                threePlayersButton.setFillColor(playerButtonBaseColor);
            }

            if (fourPlayersButton.getGlobalBounds().contains(mousePosF)) {
                fourPlayersButton.setFillColor(playerButtonHoverColor);
            }
            else {
                fourPlayersButton.setFillColor(playerButtonBaseColor);
            }
        }
        else if (gameState == GameState::InGame)
        {
            // Dice animation handling
            if (isRollingDiceAnimation)
            {
                diceAnimationElapsed += deltaTime;
                timeSinceLastDiceImageChange += deltaTime;

                if (timeSinceLastDiceImageChange >= diceImageChangeInterval)
                {
                    // Change to a random dice image
                    int randomDice = rand() % 6; // 0-5
                    diceSprite.setTexture(diceTextures[randomDice]);
                    timeSinceLastDiceImageChange = 0.0f;
                }

                if (diceAnimationElapsed >= diceAnimationDuration)
                {
                    isRollingDiceAnimation = false;

                    // Perform the actual dice roll and subsequent handling
                    int previousPosition = players[currentPlayerIndex].position;
                    int diceRoll = rand() % 6 + 1; // 1-6
                    players[currentPlayerIndex].position = (players[currentPlayerIndex].position + diceRoll) % NUM_TILES;

                    // Update player circle position
                    sf::Vector2f newPos = calculateTilePosition(players[currentPlayerIndex].position, tileSize, gridSize);
                    playerCircles[currentPlayerIndex].setPosition(
                        newPos.x + 10 + currentPlayerIndex * 15,
                        newPos.y + 10 + currentPlayerIndex * 15);

                    // Update dice image to final result
                    int diceResult = diceRoll - 1; // 0~5
                    diceSprite.setTexture(diceTextures[diceResult]);

                    // Reset popup state
                    isPopupActive = false;
                    isChoiceActive = false;
                    needToSwitchPlayer = false;
                    popupText.setString(L"");

                    // Current tile
                    Tile& currentTile = tiles[players[currentPlayerIndex].position];

                    // Handle tile events
                    if (currentTile.name == L"機會" || currentTile.name == L"命運")
                    {
                        // Random chance card
                        int randomIndex = rand() % chanceCards.size();
                        std::wstring description = chanceCards[randomIndex].first;
                        effect = chanceCards[randomIndex].second;
                        popupText.setString(wrapText(L"事件: " + description + L"\n影響: " + (effect > 0 ? L"+" : L"") + std::to_wstring(effect) + L" 點數", popupTextMaxWidth, font, 16));
                        isPopupActive = true;

                        // Update player's money or score
                        players[currentPlayerIndex].money += effect;

                        // Update player's money text
                        playerMoneyTexts[currentPlayerIndex].setString(
                            players[currentPlayerIndex].name + L": $" + std::to_wstring(players[currentPlayerIndex].money));

                        needToSwitchPlayer = true;
                    }
                    else if (currentTile.name == L"YouBike 站 1")
                    {
                        // Teleport to YouBike Station 2
                        players[currentPlayerIndex].position = 27; // index of YouBike Station 2
                        newPos = calculateTilePosition(players[currentPlayerIndex].position, tileSize, gridSize);
                        playerCircles[currentPlayerIndex].setPosition(
                            newPos.x + 10 + currentPlayerIndex * 15,
                            newPos.y + 10 + currentPlayerIndex * 15);
                        popupText.setString(wrapText(players[currentPlayerIndex].name + L" 停在了 YouBike 站 1，被傳送到 YouBike 站 2。", popupTextMaxWidth, font, 16));
                        isPopupActive = true;
                        needToSwitchPlayer = true;
                    }
                    else if (currentTile.name == L"YouBike 站 2")
                    {
                        players[currentPlayerIndex].position = 9; // index of YouBike Station 1
                        newPos = calculateTilePosition(players[currentPlayerIndex].position, tileSize, gridSize);
                        playerCircles[currentPlayerIndex].setPosition(
                            newPos.x + 10 + currentPlayerIndex * 15,
                            newPos.y + 10 + currentPlayerIndex * 15);
                        popupText.setString(wrapText(players[currentPlayerIndex].name + L" 停在了 YouBike 站 2，被傳送到 YouBike 站 1。", popupTextMaxWidth, font, 16));
                        isPopupActive = true;
                        needToSwitchPlayer = true;
                    }
                    else if (currentTile.name == L"監獄")
                    {
                        // Deduct $200 and skip next turn
                        players[currentPlayerIndex].money -= 200;
                        players[currentPlayerIndex].skipTurn = true;
                        popupText.setString(wrapText(players[currentPlayerIndex].name + L" 停在了監獄，扣除 $200，將跳過下一回合。", popupTextMaxWidth, font, 16));
                        isPopupActive = true;

                        // Update player money text
                        playerMoneyTexts[currentPlayerIndex].setString(
                            players[currentPlayerIndex].name + L": $" + std::to_wstring(players[currentPlayerIndex].money));
                        needToSwitchPlayer = true;
                    }
                    else if (currentTile.owner == -1 && currentTile.price > 0 && players[currentPlayerIndex].money >= currentTile.price)
                    {
                        // Unowned tile and player has enough money
                        popupText.setString(wrapText(L"是否購買 " + currentTile.name + L"?\n價格: $" + std::to_wstring(currentTile.price), popupTextMaxWidth, font, 16));
                        tileToBuy = currentTile.index;
                        isPopupActive = true;
                        isChoiceActive = true;
                        needToSwitchPlayer = false; // wait for player choice
                    }
                    else if (currentTile.owner == -1 && currentTile.price > 0 && players[currentPlayerIndex].money < currentTile.price)
                    {
                        // Player doesn't have enough money
                        popupText.setString(wrapText(L"你沒有足夠的金錢購買 " + currentTile.name + L".", popupTextMaxWidth, font, 16));
                        isPopupActive = true;
                        needToSwitchPlayer = true;
                    }
                    else if (currentTile.owner != -1 && currentTile.owner != currentPlayerIndex)
                    {
                        // Owned by another player, pay toll
                        int ownerIndex = currentTile.owner;
                        players[currentPlayerIndex].money -= currentTile.toll;
                        players[ownerIndex].money += currentTile.toll;

                        popupText.setString(wrapText(L"這是 " + players[ownerIndex].name + L" 的地盤！\n支付過路費: $" + std::to_wstring(currentTile.toll), popupTextMaxWidth, font, 16));
                        isPopupActive = true;
                        // Update player money texts
                        playerMoneyTexts[currentPlayerIndex].setString(
                            players[currentPlayerIndex].name + L": $" + std::to_wstring(players[currentPlayerIndex].money));
                        playerMoneyTexts[ownerIndex].setString(
                            players[ownerIndex].name + L": $" + std::to_wstring(players[ownerIndex].money));
                        needToSwitchPlayer = true;
                    }
                    else if (currentTile.owner == currentPlayerIndex)
                    {
                        // Player lands on own property, +$200
                        players[currentPlayerIndex].money += 200;
                        popupText.setString(wrapText(L"又回來了！恭喜獲得 $200！", popupTextMaxWidth, font, 16));
                        isPopupActive = true;

                        // Update player money text
                        playerMoneyTexts[currentPlayerIndex].setString(
                            players[currentPlayerIndex].name + L": $" + std::to_wstring(players[currentPlayerIndex].money));

                        needToSwitchPlayer = true;
                    }
                    else
                    {
                        // Normal tile, no special event, switch player
                        needToSwitchPlayer = true;
                    }

                    // Check if passed or landed on start
                    bool passedStart = (players[currentPlayerIndex].position < previousPosition) && (players[currentPlayerIndex].position != previousPosition);
                    if (passedStart || players[currentPlayerIndex].position == 0)
                    {
                        players[currentPlayerIndex].money += 500;
                        std::wstring startMessage = (players[currentPlayerIndex].position == 0) ?
                            players[currentPlayerIndex].name + L" 停在了起點，獲得 $500" :
                            players[currentPlayerIndex].name + L" 經過了起點，獲得 $500";

                        // Update player money text
                        playerMoneyTexts[currentPlayerIndex].setString(
                            players[currentPlayerIndex].name + L": $" + std::to_wstring(players[currentPlayerIndex].money));

                        // Update popup if active
                        if (isPopupActive)
                        {
                            // Append message if popup is already active
                            popupText.setString(wrapText(popupText.getString() + L"\n" + startMessage, popupTextMaxWidth, font, 16));
                        }
                        else
                        {
                            // If no popup, show one now
                            popupText.setString(wrapText(startMessage, popupTextMaxWidth, font, 16));
                            isPopupActive = true;
                            needToSwitchPlayer = true;
                        }
                    }

                    // If no popup is active, switch player directly
                    if (!isPopupActive && !isChoiceActive && needToSwitchPlayer)
                    {
                        needToSwitchPlayer = false;
                        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
                        currentPlayerText.setString(L"現在是 " + players[currentPlayerIndex].name + L" 的回合");
                    }
                }
            }

            // After event handling, check if we need to switch player
            if (needToSwitchPlayer && !isPopupActive && !isChoiceActive)
            {
                needToSwitchPlayer = false;
                currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
                currentPlayerText.setString(L"現在是 " + players[currentPlayerIndex].name + L" 的回合");
            }

            // Check if any player is bankrupt
            for (const auto& player : players)
            {
                if (player.money < 0)
                {
                    isGameOver = true;
                    popupText.setString(wrapText(player.name + L" 破產了，遊戲結束！", popupTextMaxWidth, font, 16));
                    isPopupActive = true;
                    gameState = GameState::GameOver;
                    break;
                }
            }

            // Check dice roll button hover
            if (button.getGlobalBounds().contains(mousePosF)) {
                button.setFillColor(buttonHoverColor);
            }
            else {
                button.setFillColor(buttonBaseColor);
            }

            // Check confirm button hover
            if (isPopupActive && !isChoiceActive) {
                if (confirmButton.getGlobalBounds().contains(mousePosF)) {
                    confirmButton.setFillColor(confirmButtonHoverColor);
                }
                else {
                    confirmButton.setFillColor(confirmButtonBaseColor);
                }
            }

            // Check yes/no buttons hover
            if (isPopupActive && isChoiceActive) {
                if (yesButton.getGlobalBounds().contains(mousePosF)) {
                    yesButton.setFillColor(yesButtonHoverColor);
                }
                else {
                    yesButton.setFillColor(yesButtonBaseColor);
                }

                if (noButton.getGlobalBounds().contains(mousePosF)) {
                    noButton.setFillColor(noButtonHoverColor);
                }
                else {
                    noButton.setFillColor(noButtonBaseColor);
                }
            }

            // Check exit button hover
            if (isGameOver && exitButton.getGlobalBounds().contains(mousePosF)) {
                exitButton.setFillColor(exitButtonHoverColor);
            }
            else {
                exitButton.setFillColor(exitButtonBaseColor);
            }
        }

        // Drawing
        window.clear(sf::Color::White);

        if (gameState == GameState::PlayerNumberSelection)
        {
            // Draw player number selection screen
            window.draw(selectPlayerTitle);
            window.draw(twoPlayersButton);
            window.draw(twoPlayersButtonText);
            window.draw(threePlayersButton);
            window.draw(threePlayersButtonText);
            window.draw(fourPlayersButton);
            window.draw(fourPlayersButtonText);
        }
        else if (gameState == GameState::InGame || gameState == GameState::GameOver)
        {
            // Draw game elements
            for (auto& tile : tiles) {
                window.draw(tile.shape);
                window.draw(tile.text);
            }
            window.draw(button);
            window.draw(buttonText);
            window.draw(diceSprite);
            for (const auto& circle : playerCircles)
                window.draw(circle);
            window.draw(currentPlayerText);
            for (const auto& moneyText : playerMoneyTexts)
                window.draw(moneyText);

            // Draw ownership marks
            for (const auto& mark : ownerMarks)
                window.draw(mark);

            // If popup is active, draw popup
            if (isPopupActive)
            {
                window.draw(popupBox);
                window.draw(popupText);

                if (isChoiceActive)
                {
                    window.draw(yesButton);
                    window.draw(yesButtonText);
                    window.draw(noButton);
                    window.draw(noButtonText);
                }
                else if (isGameOver)
                {
                    window.draw(exitButton);
                    window.draw(exitButtonText);
                }
                else
                {
                    window.draw(confirmButton);
                    window.draw(confirmButtonText);
                }
            }
        }

        window.display();
    }

    return 0;
}