#include "game.h"
const string BIG_LINE(72, '=');
const string SMALL_LINE(72, '-');
string trimText(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}
string toUpperText(string text) {
    for (size_t i = 0; i < text.length(); i++) text[i] = upperChar(text[i]);
    return text;
}
bool sameIgnoringCase(const string& first, const string& second) {
    return toUpperText(first) == toUpperText(second);
}
string currentTimestamp() {
    time_t now = time(nullptr);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&now));
    return string(buffer);
}
int readNumber(int low, int high, const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            cout << "\n[Session ended.]\n";
            exit(0);
        }
        line = trimText(line);
        if (line.empty()) {
            cout << "Input cannot be empty. Please enter a number.\n";
            continue;
        }
        char* endPointer = nullptr;
        long value = strtol(line.c_str(), &endPointer, 10);
        if (*endPointer != '\0') {
            cout << "That is not a valid number. Please try again.\n";
            continue;
        }
        if (value < low || value > high) {
            cout << "Out of range! Please enter a number between " << low << " and " << high << ".\n";
            continue;
        }
        return (int)value;
    }
}
void waitForEnter() {
    string ignored;
    if (!getline(cin, ignored)) {
        cout << "\n[Session ended.]\n";
        exit(0);
    }
}
void Victim::display() const {
    cout << "\n==================== VICTIM DOSSIER ====================\n"
        << " Name:         " << name << "\n"
        << " Role:         " << role << "\n"
        << " Nationality:  " << nationality << "\n"
        << " Credibility Required to Interview: " << requiredCredibility << "+\n"
        << " Status:       Primary Complainant (Severely Shaken)\n"
        << " Profile:      Owner of the Paris Laurent Manor & Heist Victim.\n"
        << "========================================================\n";
}
void Detective::display() const {
    string condition = "On the Brink of Dismissal!";
    if (credibility > 60) condition = "Highly Respected & Authoritative";
    else if (credibility > 30) condition = "Questioned by Manor Inhabitants";
    cout << "\n=================== DETECTIVE STATUS ===================\n"
        << " Investigator: " << name << " (" << nationality << ")\n"
        << " Rank:         " << role << "\n"
        << " Credibility:  " << credibility << " / 100\n"
        << " Turns Used:   " << turnsUsed << " / " << maxTurns
        << " (" << getTurnsRemaining() << " turns remaining)\n"
        << " Condition:    " << condition << "\n"
        << "========================================================\n";
}
void Suspect::display() const {
    cout << "\n------------------- SUSPECT PROFILE --------------------\n"
        << " Name:         " << name << "\n"
        << " Category:     " << getCategoryName() << "\n"
        << " Role:         " << role << "\n"
        << " Nationality:  " << nationality << "\n"
        << " Min Cred:     " << getRequiredCredibility() << "+\n"
        << " Interviewed:  " << (interviewed ? "YES" : "NO") << "\n"
        << " Alibi:        " << alibi << "\n"
        << "--------------------------------------------------------\n";
}
void BusinessRelations::display() const {
    cout << "\n[BUSINESS RELATIONS DOSSIER]\n";
    Suspect::display();
    cout << " Commercial Ties: Has access to corporate ledgers and financial audits.\n";
}
void HouseHelp::display() const {
    cout << "\n[HOUSE HELP DOSSIER]\n";
    Suspect::display();
    cout << " Domestic Access: Knows secret passageways, kitchen corridors, and gate locks.\n";
}
void CloseRelations::display() const {
    cout << "\n[CLOSE RELATIONS DOSSIER]\n";
    Suspect::display();
    cout << " Family Heritage: High social standing; deeply defensive regarding inheritance rights.\n";
}
void PA::display() const {
    cout << "\n==================== PA DOSSIER (DIAMOND RESOLVED) ====================\n"
        << " Name:         " << name << "\n"
        << " Category:     " << getCategoryName() << "\n"
        << " Role:         " << role << " (" << nationality << ")\n"
        << " Credibility:  Requires " << getRequiredCredibility() << "+ credibility\n"
        << " Dual Status:  Authorized corporate signatory AND direct blood relative (Niece)\n"
        << " Alibi:        " << alibi << "\n"
        << " Notes:        Holds digital keys to the master schedule and confidential files.\n"
        << "=======================================================================\n";
}
int TileRack::countLetter(char letter) const {
    int total = 0;
    for (int i = 0; i < tiles.size(); i++)
        if (tiles[i] == letter) total++;
    return total;
}
void TileRack::addTile(const Tile& tile) {
    if (tiles.size() >= maxTiles)
        throw RackFullException("Cannot add tile '" + string(1, tile.letter) + "': the rack is full.");
    tiles.push_back(tile);
}
bool TileRack::removeLetter(char letter) {
    for (int i = 0; i < tiles.size(); i++) {
        if (tiles[i] == letter) {
            tiles.removeAt(i);
            return true;
        }
    }
    return false;
}
Tile TileRack::removeTileAt(int index) {
    Tile removed = tiles[index];
    tiles.removeAt(index);
    return removed;
}
TileRack TileRack::operator+(const TileRack& other) const {
    TileRack merged(maxTiles + other.maxTiles);
    for (int i = 0; i < tiles.size(); i++) merged.addTile(tiles[i]);
    for (int i = 0; i < other.tiles.size(); i++) merged.addTile(other.tiles[i]);
    return merged;
}
bool TileRack::operator==(const TileRack& other) const {
    if (tiles.size() != other.tiles.size()) return false;
    for (int i = 0; i < tiles.size(); i++)
        if (countLetter(tiles[i].letter) != other.countLetter(tiles[i].letter)) return false;
    return true;
}
ScrabbleBoard::ScrabbleBoard() {
    reset();
}
void ScrabbleBoard::reset() {
    for (int row = 0; row < 2; row++) {
        rows[row].clear();
        for (int slot = 0; slot < getRowLength((WordRow)row); slot++) rows[row].push_back(Tile());
    }
}
int ScrabbleBoard::getRowLength(WordRow row) const {
    return (row == Word_1) ? 7 : 8;
}
void ScrabbleBoard::checkPosition(WordRow row, int position) const {
    int length = getRowLength(row);
    if (position < 0 || position >= length)
        throw InvalidPositionException("Invalid board position " + to_string(position + 1)
            + ". Expected a slot between 1 and " + to_string(length));
}
void ScrabbleBoard::placeTile(WordRow row, int position, char letter) {
    checkPosition(row, position);
    rows[row][position] = Tile(upperChar(letter), true);
}
bool ScrabbleBoard::removeTile(WordRow row, int position, char& removedLetter) {
    checkPosition(row, position);
    if (!rows[row][position].filled) return false;
    removedLetter = rows[row][position].letter;
    rows[row][position] = Tile();
    return true;
}
bool ScrabbleBoard::isSlotFilled(WordRow row, int position) const {
    checkPosition(row, position);
    return rows[row][position].filled;
}
bool ScrabbleBoard::isRowFull(WordRow row) const {
    for (int slot = 0; slot < getRowLength(row); slot++)
        if (!rows[row][slot].filled) return false;
    return true;
}
string ScrabbleBoard::getRowText(WordRow row) const {
    string text = "";
    for (int slot = 0; slot < getRowLength(row); slot++)
        text += rows[row][slot].filled ? rows[row][slot].letter : '_';
    return text;
}
void ScrabbleBoard::validateRow(WordRow row, const string& expectedWord) const {
    if (!isRowFull(row)) throw InvalidWordException("Cannot validate: the row still has empty slots.");
    string formed = getRowText(row);
    if (!sameIgnoringCase(formed, expectedWord))
        throw InvalidWordException("Submitted word '" + formed + "' is incorrect! Penalties applied.");
}
void ScrabbleBoard::display() const {
    string titles[2] = { "Crime Scene Locations - 7 Letters", "Mansion Suspects - 8 Letters" };
    cout << "\n" << BIG_LINE << "\n"
        << "                     THE LOCKER HEIST: SCRABBLE BOARD                   \n"
        << BIG_LINE << "\n";
    for (int row = 0; row < 2; row++) {
        cout << " ROW " << (row + 1) << " [" << titles[row] << "]:\n Slot Pos:  ";
        for (int slot = 0; slot < rows[row].size(); slot++) cout << " [ " << (slot + 1) << " ]";
        cout << "\n Current:   ";
        for (int slot = 0; slot < rows[row].size(); slot++) cout << " " << rows[row][slot] << " ";
        cout << "\n\n";
    }
    cout << BIG_LINE << "\n";
}
void FileManager::writeFile(const string& filename, const string& text) {
    ofstream file(filename);
    file << text;
}
bool FileManager::fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}
void FileManager::makeSureFilesExist() {
    if (!fileExists("characters.txt")) createCharactersFile();
    if (!fileExists("locations.txt")) createLocationsFile();
    if (!fileExists("clue.txt")) createClueFile();
    if (!fileExists("rules.txt")) createRulesFile();
    if (!fileExists("about.txt")) createAboutFile();
}
DynamicArray<string> FileManager::splitFields(const string& line) {
    DynamicArray<string> fields;
    stringstream stream(line);
    string field;
    while (getline(stream, field, '|')) fields.push_back(field);
    return fields;
}
void FileManager::appendScore(const LeaderboardEntry& entry) {
    ofstream file("scoreBoard.txt", ios::app);
    file << entry.playerName << "|" << entry.score << "|" << entry.credibility << "|"
        << entry.turnsUsed << "|" << entry.outcome << "|" << entry.date << "\n";
}
void FileManager::loadScores(DynamicArray<LeaderboardEntry>& entries) {
    entries.clear();
    ifstream file("scoreBoard.txt");
    string line;
    while (getline(file, line)) {
        line = trimText(line);
        if (line.empty() || line[0] == '#') continue;
        DynamicArray<string> fields = splitFields(line);
        if (fields.size() < 6) continue;
        entries.push_back(LeaderboardEntry(fields[0], atoi(fields[1].c_str()), atoi(fields[2].c_str()),
            atoi(fields[3].c_str()), fields[4], fields[5]));
    }
}
void FileManager::createCharactersFile() {
    writeFile("characters.txt", R"TXT(# THE LOCKER HEIST - CHARACTER DEFINITIONS FILE
# Format: TYPE|NAME|ROLE|NATIONALITY|REQ_CRED|CLUE_LETTER|BOARD_POS|ALIBI|STATEMENT|CONFESSION
VICTIM|Henri Laurent|Art Dealer & Gem Merchant|French|80|*|0|Resting in the master suite before gala speech|Someone cracked Locker #77! The Star of Versailles is gone!|N/A
BUSINESS|Arthur Sterling|Senior Investment Partner|British|50|B|0|Reviewing offshore tax balance sheets in west parlor|Henri and I argued over our venture capital liquidation, but I am no thief.|I orchestrated the combination heist! My firm was bankrupt and Henri refused to bail me out!
BUSINESS|Marcus Vance|Venture Capitalist Partner|American|50|E|1|Smoking a hand-rolled Cuban cigar on the terrace|I invest in gold and art, Inspector. Breaking safes with crowbars is beneath me.|I financed the thermal drill! Henri was going to blow the whistle on our offshore shell corporations!
HELP|Pierre Dubois|Head Chef & Kitchen Master|French|0|T|2|Glazing chocolate souffles in the underground kitchen|I was tasting truffle sauces all night! The pantry door was unlocked.|Henri threatened to fire me and ruin my culinary reputation! I took the keycard from his coat!
HELP|Boris Richter|Night Security Guard|German|0|R|3|Checking perimeter security gates during gala changeover|The security cameras on the mezzanine malfunctioned for twenty minutes.|I disabled the CCTV recording loop! A syndicate offered me five hundred thousand euros to turn a blind eye!
HELP|Elena Rostova|Head Housekeeper & Maid|Russian|0|A|4|Arranging crystal flower vases in the grand ballroom|I served champagne flutes to society guests until midnight.|I dusted Henri's study and copied the safe combination scribbled inside his antique leather ledger!
FAMILY|Chloe Laurent|Eldest Daughter & Heiress|French|75|Y|5|Dancing the waltz in the central hall with foreign dignitaries|Father promised the Star of Versailles as my dowry anyway.|Father disinherited me for gambling in Monte Carlo! I took what was rightfully my family birthright!
FAMILY|Julian Laurent|Owner's Son & Prodigal Heir|French|75|A|6|Sipping vintage champagne by the marble fountain garden|I was laughing with investors by the fountain. My allowance is ample.|The casino syndicate in Macao had a bounty on my head! I needed the diamond to save my own life!
PA|Vivienne Moreau|Personal Assistant & Niece|French-Swiss|60|L|7|Reconciling gala vendor invoices in mezzanine office|Uncle Henri trusted me with his calendar, but I had no reason to touch the vault.|Uncle Henri promised me an equal partnership after twenty years of service! But he revised his will to leave everything to Julian! I took the Star of Versailles and the ledger to escape to Zurich!
)TXT");
}
void FileManager::createLocationsFile() {
    writeFile("locations.txt", R"TXT(# THE LOCKER HEIST - CRIME SCENE LOCATIONS FILE
# Format: NAME|LETTER|BOARD_POS|ROW|DESCRIPTION
Master Bedroom Vault|L|0|0|Inside Henri's walk-in titanium vault, Locker #77 stands wide open. Scratched upon the brass latch is a stamped letter 'L'.
Private Study & Desk|O|1|0|Henri's mahogany desk is ransacked. A scorched piece of blotting paper inside the desk blotter reveals an imprinted letter 'O'.
Wine Cellar Archive|C|2|0|Behind a vintage 1945 Bordeaux cask, hidden muddy footsteps lead toward an air grate. A dropped monogrammed cufflink marks slot 'C'.
Conservatory Garden|K|3|0|A shattered stained-glass window panel facing the courtyard. Muddy tire treads and a discarded lockpick bearing engraving 'K'.
Grand Library Archives|E|4|0|Rare first-edition books are overturned on the parquet floor. An open safe manual holds a silk ribbon bookmark on letter 'E'.
Security Monitoring Room|R|5|0|The digital feeds were looped at 11:45 PM. The override security console terminal flashes an encryption code 'R'.
Underground Safe Tunnel|S|6|0|A damp brick passage running beneath the fountain. Fresh crowbar marks and a chalk-drawn symbol 'S' point to the escape hatch.
)TXT");
}
void FileManager::createClueFile() {
    writeFile("clue.txt", R"TXT(========================================================================
             THE LOCKER HEIST: DETECTIVE CASE BRIEFING & CLUE JOURNAL
========================================================================
CASE FILE #1924-HEIST: PARIS LAURENT MANOR
VICTIM: Monsieur Henri Laurent (Wealthy French Art & Gem Merchant)
CRIME: Breach of High-Security Titanium Locker #77 during Winter Gala.
STOLEN ASSETS:
 1. The Star of Versailles (50-carat blue diamond valued at 12M Euros).
 2. Laurent Corporation Offshore Financial Cipher Ledger.

THE TWO MASTER CIPHERS:
 * CIPHER 1 (7 Letters): Discovered across the 7 Manor Crime Scene Locations.
   (Points toward the method and container of the robbery: 'LOCKERS')
 * CIPHER 2 (8 Letters): Discovered across the 8 Interrogated Suspects.
   (Points toward the dark motive behind the family conspiracy: 'BETRAYAL')

INVESTIGATION PROTOCOL:
 - Search all 7 locations to gather Word 1 tiles.
 - Interrogate all 8 suspects to gather Word 2 tiles.
 - Interview the Victim (requires Credibility >= 80) for overarching insight.
 - Observe credibility thresholds: House Help (0+), Business (50+), PA (60+), Close Relations (75+).
========================================================================
)TXT");
}
void FileManager::createRulesFile() {
    writeFile("rules.txt", R"TXT(========================================================================
                      THE LOCKER HEIST - OFFICIAL RULES MANUAL
========================================================================
1. DETECTIVE CAPABILITIES & LIMITS:
   - You begin with 100 Credibility and a maximum of 25 Turns.
   - Every investigation action or interrogation consumes 1 Turn.
   - If your Turns reach 25 or your Credibility falls to 0, the case collapses
     and the game ends in DEFEAT.

2. CRIME SCENE INVESTIGATIONS (WORD 1 - 7 LETTERS):
   - Each unexplored location yields 1 Scrabble tile for Word 1.
   - Locations cannot be searched twice (AlreadyVisitedException).

3. SUSPECT INTERROGATIONS (WORD 2 - 8 LETTERS):
   - Suspects belong to specific social tiers with credibility requirements:
     * House Help (Pierre, Boris, Elena): Credibility >= 0
     * Business Relations (Arthur, Marcus): Credibility >= 50
     * Personal Assistant (Vivienne Moreau): Credibility >= 60
     * Close Relations (Chloe, Julian): Credibility >= 75
     * Victim (Henri Laurent): Credibility >= 80
   - Attempting to interrogate a character without meeting their required
     credibility triggers LowCredibilityException, costs 1 Turn, and penalizes
     you -5 Credibility!

4. SCRABBLE BOARD CIPHER SOLVING:
   - Tiles gathered from locations and suspects are stored in your Tile Rack.
   - You can place tiles into board positions (Row 1: 1-7, Row 2: 1-8).
   - When a row is full, you can validate the word.
   - An incorrect word submission triggers InvalidWordException, costs 1 Turn,
     and inflicts a severe -10 Credibility penalty!

5. ACCUSING THE CULPRIT & WIN CONDITIONS:
   - At the start of every game, one of the 8 suspects is dynamically chosen
     as the true mastermind culprit.
   - To WIN, you must identify the correct culprit and successfully solve
     both Scrabble ciphers.
   - A false accusation inflicts -20 Credibility and expends 2 extra turns!
========================================================================
)TXT");
}

void FileManager::createAboutFile() {

    writeFile("about.txt", R"TXT(========================================================================
                   THE LOCKER HEIST - ABOUT & ARCHITECTURE
========================================================================
Project: The Locker Heist - Production C++ Mystery Console Application

Architecture Highlights:

 * ZERO STL CONTAINER / ALGORITHM COMPLIANCE:
   Built from the ground up without <vector>, <list>, <map>, or <algorithm>.
   All dynamic sizing is governed by a custom DynamicArray<T> template with
   deep-copy semantics, Rule of Three compliance, and dynamic reallocation.

 * DIAMOND INHERITANCE RESOLVED:
   The Personal Assistant (PA) class derives from both BusinessRelations and
   CloseRelations, utilizing virtual base inheritance (virtual public Suspect
   and virtual public Character) to eliminate duplicate base instances.

 * SINGLETON PATTERN:
   GameEngine is structured as a static Meyers Singleton.

 * OPERATOR OVERLOADING:
   - operator<< and operator>> for stream input/output.
   - operator== for Tile and TileRack letter comparisons.
   - operator+ for TileRack merging operations.

 * FRIEND CLASS RELATIONSHIPS:
   GameEngine declared as friend of Character for direct privileged access.

 * CUSTOM EXCEPTION HANDLING:
   GameException base class with derived InvalidWordException, RackFullException,
   InvalidPositionException, LowCredibilityException, AlreadyVisitedException.

========================================================================
                         PROJECT INFORMATION
========================================================================

Instructor:
Sir Shahzad Tanveer

Project Team:
Name                              Roll Number
------------------------------------------------
Ali Raza                          25F0509
Ahsan Ibrahim                     25F0643
Rehan Zulifqar                    25F0678

========================================================================
                    END OF ABOUT THE GAME
========================================================================
)TXT");

}

GameEngine::GameEngine()
    : state(GameState::Menu), detective(nullptr), victim(nullptr),
    word1("LOCKERS"), word2("BETRAYAL"), culpritIndex(0),
    word1Solved(false), word2Solved(false), cipherMessageShown(false) {
}
GameEngine::~GameEngine() {
    cleanUp();
}
GameEngine& GameEngine::getInstance() {
    static GameEngine instance;
    return instance;
}
void GameEngine::cleanUp() {
    delete detective;
    detective = nullptr;
    delete victim;
    victim = nullptr;
    for (int i = 0; i < suspects.size(); i++) delete suspects[i];
    suspects.clear();
    locations.clear();
    journal.clear();
    rack.clear();
}
void GameEngine::run() {
   
    srand((unsigned int)time(nullptr));
    FileManager::makeSureFilesExist();

    cout << "\n" << BIG_LINE << "\n"
        << "                  WELCOME TO THE LOCKER HEIST                           \n"
        << "              A Master Detective Mystery Console Adventure              \n"
        << BIG_LINE << "\n";
    while (state != GameState::Exit) {
        system("cls");
        showMainMenu();
        int choice = readNumber(1, 5, "Enter menu selection (1-5): ");
        if (choice == 1) startNewGame();
        else if (choice == 2) showLeaderboard();
        else if (choice == 3) showTextFile("about.txt");
        else if (choice == 4) showTextFile("rules.txt");
        else {
            state = GameState::Exit;
            cout << "\nThank you for playing The Locker Heist. Case closed.\n";
        }
    }
}
void GameEngine::showMainMenu() {
    cout << "\n" << BIG_LINE << "\n"
        << "                              MAIN MENU                                 \n"
        << BIG_LINE << "\n"
        << " [1] New Mystery Investigation (Start Game)\n"
        << " [2] Detective Hall of Fame (Leaderboard)\n"
        << " [3] About the Game\n"
        << " [4] Inspector's Field Manual (Game Rules)\n"
        << " [5] Exit Application\n"
        << BIG_LINE << "\n";
}
void GameEngine::showTextFile(const string& filename) {
    system("cls");
    FileManager::makeSureFilesExist();
    ifstream file(filename);
    string line;
    cout << "\n";
    while (getline(file, line)) cout << line << "\n";
    cout << "Press ENTER to return to Main Menu...";
    waitForEnter();
}
void GameEngine::showLeaderboard() {
    system("cls");
    DynamicArray<LeaderboardEntry> entries;
    FileManager::loadScores(entries);
    cout << "\n" << BIG_LINE << "\n"
        << "                   DETECTIVE HALL OF FAME (LEADERBOARD)                 \n"
        << BIG_LINE << "\n";
    if (entries.isEmpty()) {
        cout << " No previous case records found. Solve a mystery to register your name!\n";
    }
    else {
        for (int i = 0; i < entries.size() - 1; i++)
            for (int j = 0; j < entries.size() - i - 1; j++)
                if (entries[j].score < entries[j + 1].score) customSwap(entries[j], entries[j + 1]);
        cout << left << setw(6) << "RANK" << setw(20) << "DETECTIVE" << setw(10) << "SCORE"
            << setw(12) << "CREDIBILITY" << setw(8) << "TURNS" << setw(10) << "OUTCOME"
            << setw(18) << "DATE" << "\n" << SMALL_LINE << "\n";
        int rowsToShow = customMin(entries.size(), 10);
        for (int i = 0; i < rowsToShow; i++) {
            cout << left << setw(6) << (i + 1) << setw(20) << entries[i].playerName
                << setw(10) << entries[i].score << setw(12) << entries[i].credibility
                << setw(8) << entries[i].turnsUsed << setw(10) << entries[i].outcome
                << setw(18) << entries[i].date << "\n";
        }
    }
    cout << BIG_LINE << "\nPress ENTER to return to Main Menu...";
    waitForEnter();
}
void GameEngine::startNewGame() {
    cleanUp();
    system("cls");
    FileManager::makeSureFilesExist();
    cout << "\n=================== NEW DETECTIVE REGISTRATION ===================\n"
        << "Enter your Detective's Name (or press ENTER for 'Raymond Vance'): ";
    string playerName;
    getline(cin, playerName);
    playerName = trimText(playerName);
    if (playerName.empty()) playerName = "Raymond Vance";
    detective = new Detective(playerName);
    board.reset();
    word1Solved = false;
    word2Solved = false;
    cipherMessageShown = false;
    loadLocations();
    loadCharacters();
    pickCulprit();
    state = GameState::Playing;
    playGame();
}
void GameEngine::loadLocations() {
    ifstream file("locations.txt");
    string line;
    while (getline(file, line)) {
        line = trimText(line);
        if (line.empty() || line[0] == '#') continue;
        DynamicArray<string> fields = FileManager::splitFields(line);
        if (fields.size() < 5) continue;
        char letter = fields[1].empty() ? ' ' : fields[1][0];
        locations.push_back(Location(fields[0], letter, atoi(fields[2].c_str()), fields[4]));
    }
}
void GameEngine::loadCharacters() {
    ifstream file("characters.txt");
    string line;
    while (getline(file, line)) {
        line = trimText(line);
        if (line.empty() || line[0] == '#') continue;
        DynamicArray<string> fields = FileManager::splitFields(line);
        if (fields.size() < 10) continue;
        string type = fields[0];
        string name = fields[1];
        string role = fields[2];
        string nationality = fields[3];
        int minCredibility = atoi(fields[4].c_str());
        char letter = fields[5].empty() ? ' ' : fields[5][0];
        int position = atoi(fields[6].c_str());
        string alibi = fields[7];
        string statement = fields[8];
        string confession = fields[9];
        if (type == "VICTIM")
            victim = new Victim(name, role, nationality, minCredibility, statement);
        else if (type == "BUSINESS")
            suspects.push_back(new BusinessRelations(name, role, nationality, letter, position, alibi, statement, confession));
        else if (type == "HELP")
            suspects.push_back(new HouseHelp(name, role, nationality, letter, position, alibi, statement, confession));
        else if (type == "FAMILY")
            suspects.push_back(new CloseRelations(name, role, nationality, letter, position, alibi, statement, confession));
        else if (type == "PA")
            suspects.push_back(new PA(name, role, nationality, letter, position, alibi, statement, confession));
    }
    if (victim == nullptr) victim = new Victim();
}
void GameEngine::pickCulprit() {
    if (suspects.isEmpty()) return;
    culpritIndex = rand() % suspects.size();
    suspects[culpritIndex]->setCulprit(true);
}
void GameEngine::playGame() {
    system("cls");
    cout << "\n>>> CASE COMMENCED: The Winter Gala Heist at Paris Laurent Manor <<<\n"
        << "Locker #77 has been breached. The Star of Versailles and ledger are missing.\n"
        << "Search locations for Word 1 (7 letters) and interrogate suspects for Word 2 (8 letters).\n";
    while (state == GameState::Playing) {
        showDashboard();
        int action = readNumber(1, 8, "Choose Action (1-8): ");
        try {
            if (action == 1) searchLocation();
            else if (action == 2) interrogateSuspect();
            else if (action == 3) showJournal();
            else if (action == 4) manageBoard();
            else if (action == 5) showRackMerge();
            else if (action == 6) accuseCulprit();
            else if (action == 7) showDetectiveFile();
            else forfeitGame();
        }
        catch (const LowCredibilityException& error) {
            cout << "\n[CREDIBILITY FAILURE] " << error.what() << "\n";
            detective->loseCredibility(5);
            detective->useTurns(1);
            cout << "Penalty Applied: -5 Credibility, +1 Turn consumed.\n";
        }
        catch (const AlreadyVisitedException& error) {
            cout << "\n[REDUNDANT ACTION] " << error.what() << "\n"
                << "No turns or credibility lost. Please pick an unexplored target.\n";
        }
        catch (const InvalidWordException& error) {
            cout << "\n[CIPHER ERROR] " << error.what() << "\n";
            detective->loseCredibility(10);
            detective->useTurns(1);
            cout << "Severe Penalty Applied: -10 Credibility, +1 Turn consumed.\n";
        }
        catch (const GameException& error) {
            cout << "\n[GAME ERROR] " << error.what() << "\n";
        }
        checkGameEnd();
    }
    finishGame();
}
void GameEngine::showDashboard() {
    string word1Status = word1Solved ? "SOLVED: " + word1 : board.getRowText(Word_1);
    string word2Status = word2Solved ? "SOLVED: " + word2 : board.getRowText(Word_2);
    cout << "\n" << BIG_LINE << "\n"
        << "                     THE LOCKER HEIST: DETECTIVE DASHBOARD              \n"
        << BIG_LINE << "\n"
        << " Turn: " << detective->getTurnsUsed() << "/" << detective->getMaxTurns()
        << " | Credibility: " << detective->getCredibility() << "/100\n"
        << " Word 1 Status: [" << word1Status << "]\n"
        << " Word 2 Status: [" << word2Status << "]\n"
        << " Active Tile Rack: " << rack << "\n"
        << SMALL_LINE << "\n"
        << " [1] Search Crime Scene Location (Collect Word 1 Tiles)\n"
        << " [2] Interrogate Suspect or Interview Victim (Collect Word 2 Tiles)\n"
        << " [3] Inspect Clue Log & Player Tile Rack\n"
        << " [4] Scrabble Board Operations (Place / Remove / Validate Words)\n"
        << " [5] Tile Rack Merger Demonstration (Overloaded operator+)\n"
        << " [6] Accuse the Mastermind Culprit (Solve the Mystery)\n"
        << " [7] View Detailed Detective Case File\n"
        << " [8] Forfeit Investigation & Return to Main Menu\n"
        << BIG_LINE << "\n";
}
void GameEngine::searchLocation() {
    system("cls");
    cout << "\n------------------- CRIME SCENE LOCATIONS --------------------\n";
    for (int i = 0; i < locations.size(); i++) {
        cout << " [" << (i + 1) << "] " << locations[i].name
            << (locations[i].searched ? " [ALREADY SEARCHED]" : " [UNEXPLORED]") << "\n";
    }
    cout << " [0] Cancel and return\n" << SMALL_LINE << "\n";
    int choice = readNumber(0, locations.size(), "Select location to search: ");
    if (choice == 0) return;
    Location& place = locations[choice - 1];
    if (place.searched)
        throw AlreadyVisitedException("Location '" + place.name + "' has already been searched!");
    detective->useTurns(1);
    place.searched = true;
    cout << "\n[INVESTIGATION UNDERWAY] Detective searches " << place.name << "...\n"
        << "Observation: " << place.description << "\n";
    rack.addTile(Tile(place.letter, true));
    journal.push_back(Clue(place.letter, place.position, Word_1, place.name, place.description));
    cout << ">>> EVIDENCE SECURED: Found Tile [" << place.letter
        << "] belonging to Word 1, Board Slot " << (place.position + 1) << "! <<<\n"
        << "The tile has been transferred to your Tile Rack.\n";
}
void GameEngine::interrogateSuspect() {
    system("cls");
    cout << "\n------------------- MANOR INHABITANTS & SUSPECTS -------------------\n"
        << " [0] " << victim->getName() << " (Victim - " << victim->getRole() << ") [Req Cred: "
        << victim->getRequiredCredibility() << "+]\n";
    for (int i = 0; i < suspects.size(); i++) {
        cout << " [" << (i + 1) << "] " << suspects[i]->getName()
            << " (" << suspects[i]->getRole() << ") - Category: " << suspects[i]->getCategoryName()
            << " [Req Cred: " << suspects[i]->getRequiredCredibility() << "+]"
            << (suspects[i]->isInterviewed() ? " [INTERVIEWED]" : " [PENDING]") << "\n";
    }
    cout << " [-1] Cancel and return\n" << SMALL_LINE << "\n";
    int choice = readNumber(-1, suspects.size(), "Select character to interview (-1 to cancel): ");
    if (choice == -1) return;
    if (choice == 0) {
        if (detective->getCredibility() < victim->getRequiredCredibility())
            throw LowCredibilityException("Victim Henri Laurent demands an inspector of supreme credibility (80+) before speaking!");
        detective->useTurns(1);
        victim->display();
        cout << "\nHenri Laurent speaks:\n\"" << victim->getTestimony() << "\"\n"
            << "\"I suspect someone with intimate access to both my business contracts and family keys.\n"
            << "Pay special attention to who has alibis that cannot be corroborated by servants!\"\n";
        return;
    }
    Suspect* suspect = suspects[choice - 1];
    if (suspect->isInterviewed())
        throw AlreadyVisitedException("Suspect " + suspect->getName() + " has already given their testimony.");
    if (detective->getCredibility() < suspect->getRequiredCredibility())
        throw LowCredibilityException(suspect->getName() + " sneers and refuses to answer: Credibility must be at least "
            + to_string(suspect->getRequiredCredibility()) + ".");
    detective->useTurns(1);
    suspect->markInterviewed();
    suspect->display();
    cout << "\nSworn Statement:\n\"" << suspect->getStatement() << "\"\n";
    if (suspect->isTheCulprit())
        cout << "[OBSERVATION NOTE] " << suspect->getName()
        << " appears noticeably fidgety, avoiding eye contact when Locker #77 is mentioned.\n";
    else
        cout << "[OBSERVATION NOTE] " << suspect->getName()
        << " speaks calmly, their alibi appears consistent with staff records.\n";
    rack.addTile(Tile(suspect->getLetter(), true));
    journal.push_back(Clue(suspect->getLetter(), suspect->getPosition(), Word_2,
        suspect->getName() + " (" + suspect->getCategoryName() + ")", suspect->getStatement()));
    cout << ">>> TESTIMONY EXTRACTED: Received Tile [" << suspect->getLetter()
        << "] belonging to Word 2, Board Slot " << (suspect->getPosition() + 1) << "! <<<\n"
        << "The tile has been transferred to your Tile Rack.\n";
}
void GameEngine::showJournal() {
    system("cls");
    cout << "\n" << BIG_LINE << "\n"
        << "                    DETECTIVE CASE JOURNAL & EVIDENCE LOG               \n"
        << BIG_LINE << "\n";
    if (journal.isEmpty()) cout << "No clues or evidence tiles have been logged yet.\n";
    for (int i = 0; i < journal.size(); i++) cout << " [" << (i + 1) << "] " << journal[i] << "\n";
    cout << "\nActive Tile Rack Contents (" << rack.getCount() << " tiles): " << rack << "\n"
        << BIG_LINE << "\nPress ENTER to return to dashboard...";
    waitForEnter();
}
void GameEngine::manageBoard() {
    system("cls");
    int operation = -1;
    while (operation != 0) {
        board.display();
        cout << "\nActive Tile Rack: " << rack << "\n"
            << "----------------- SCRABBLE OPERATIONS -----------------\n"
            << " [1] Place Tile from Rack onto Board\n"
            << " [2] Remove Tile from Board back to Rack\n"
            << " [3] Validate Row 1 (Locations Cipher: 7 Letters)\n"
            << " [4] Validate Row 2 (Suspects Cipher: 8 Letters)\n"
            << " [5] Auto-Place All Matching Clues (Detective Synthesis)\n"
            << " [0] Return to Main Dashboard\n"
            << "-------------------------------------------------------\n";
        operation = readNumber(0, 5, "Choose Scrabble operation: ");
        if (operation == 1) placeTileOnBoard();
        else if (operation == 2) removeTileFromBoard();
        else if (operation == 3) solveRow(Word_1);
        else if (operation == 4) solveRow(Word_2);
        else if (operation == 5) autoPlaceTiles();
    }
}
void GameEngine::placeTileOnBoard() {
    if (rack.isEmpty()) {
        cout << "Your Tile Rack is empty! Search locations or interrogate suspects to find tiles.\n";
        return;
    }
    cout << "\nSelect tile number from rack to place:\n";
    for (int i = 0; i < rack.getCount(); i++) cout << " [" << (i + 1) << "] " << rack.getTile(i).letter << "  ";
    cout << "\n";
    int tileIndex = readNumber(1, rack.getCount(), "Tile selection: ") - 1;
    char letter = rack.getTile(tileIndex).letter;
    int rowNumber = readNumber(1, 2, "Select Row (1 for Word 1, 2 for Word 2): ");
    WordRow row = (rowNumber == 1) ? Word_1 : Word_2;
    int slotCount = board.getRowLength(row);
    int slot = readNumber(1, slotCount, "Select Slot Position (1 to " + to_string(slotCount) + "): ") - 1;
    if (board.isSlotFilled(row, slot)) {
        char oldLetter = ' ';
        board.removeTile(row, slot, oldLetter);
        rack.addTile(Tile(oldLetter, true));
        cout << "Replaced existing tile [" << oldLetter << "] and returned it to your rack.\n";
    }
    rack.removeTileAt(tileIndex);
    board.placeTile(row, slot, letter);
    cout << "Placed tile [" << letter << "] onto Row " << rowNumber << ", Slot " << (slot + 1) << ".\n";
}
void GameEngine::removeTileFromBoard() {
    int rowNumber = readNumber(1, 2, "Select Row to remove tile from (1 or 2): ");
    WordRow row = (rowNumber == 1) ? Word_1 : Word_2;
    int slotCount = board.getRowLength(row);
    int slot = readNumber(1, slotCount, "Select Slot Position to clear (1 to " + to_string(slotCount) + "): ") - 1;
    char removedLetter = ' ';
    if (board.removeTile(row, slot, removedLetter)) {
        rack.addTile(Tile(removedLetter, true));
        cout << "Tile [" << removedLetter << "] removed from board and returned to rack.\n";
    }
    else {
        cout << "Slot " << (slot + 1) << " was already empty.\n";
    }
}
void GameEngine::autoPlaceTiles() {
    int placedCount = 0;
    for (int i = 0; i < journal.size(); i++) {
        char letter = journal[i].letter;
        WordRow row = journal[i].row;
        int position = journal[i].position;
        if (!board.isSlotFilled(row, position) && rack.hasLetter(letter)) {
            rack.removeLetter(letter);
            board.placeTile(row, position, letter);
            placedCount++;
        }
    }
    cout << "Detective Synthesis complete: " << placedCount << " clue tiles automatically placed on the board.\n";
}
void GameEngine::solveRow(WordRow row) {
    bool alreadySolved = (row == Word_1) ? word1Solved : word2Solved;
    string target = (row == Word_1) ? word1 : word2;
    int rowNumber = row + 1;
    if (alreadySolved) {
        cout << "Row " << rowNumber << " is already solved as: '" << target << "'!\n";
        return;
    }
    board.validateRow(row, target);
    if (row == Word_1) word1Solved = true;
    else word2Solved = true;
    detective->gainCredibility(10);
    cout << "\n>>> CONGRATULATIONS! Word " << rowNumber << " correctly solved as '" << target
        << "'! +10 Credibility awarded! <<<\n";
}
void GameEngine::showRackMerge() {
    cout << "\n" << BIG_LINE << "\n"
        << "                  TILE RACK OPERATOR+ MERGE DEMONSTRATION               \n"
        << BIG_LINE << "\n"
        << "This demonstrates the overloaded operator+ for merging TileRack objects.\n\n";
    TileRack archiveRack(10);
    archiveRack.addTile(Tile('X', true));
    archiveRack.addTile(Tile('Y', true));
    archiveRack.addTile(Tile('Z', true));
    cout << "Current Detective Rack: " << rack << " (Count: " << rack.getCount() << ")\n"
        << "Archive Evidence Rack:  " << archiveRack << " (Count: " << archiveRack.getCount() << ")\n\n";
    TileRack mergedRack = rack + archiveRack;
    cout << ">>> Result of (Detective Rack + Archive Evidence Rack): <<<\n"
        << "Merged Tile Rack: " << mergedRack << " (Total Count: " << mergedRack.getCount() << ")\n"
        << "Operator== check (rack == mergedRack): " << ((rack == mergedRack) ? "TRUE" : "FALSE") << "\n"
        << BIG_LINE << "\nPress ENTER to return...";
    waitForEnter();
}
void GameEngine::accuseCulprit() {
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        << "                         FINAL CULPRIT ACCUSATION                       \n"
        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        << "WARNING: You are about to name the mastermind of the Paris Locker Heist.\n"
        << "A false accusation inflicts a massive -20 Credibility penalty and costs\n"
        << "2 precious turns! Ensure your deduction is airtight.\n\n";
    for (int i = 0; i < suspects.size(); i++) {
        cout << " [" << (i + 1) << "] " << suspects[i]->getName() << " ("
            << suspects[i]->getRole() << ") - " << suspects[i]->getCategoryName() << "\n";
    }
    cout << " [0] Cancel accusation\n" << SMALL_LINE << "\n";
    int choice = readNumber(0, suspects.size(), "Name the culprit (1-8, 0 to cancel): ");
    if (choice == 0) return;
    Suspect* accused = suspects[choice - 1];
    cout << "\nYou step into the manor drawing room and point your finger directly at "
        << accused->getName() << "!\n";
    if (accused->isTheCulprit()) {
        cout << "\n" << BIG_LINE << "\n"
            << "                        THE CULPRIT BREAKS DOWN!                        \n"
            << BIG_LINE << "\n"
            << accused->getName() << " turns pale, trembling, and collapses into an armchair!\n\n"
            << "CONFESSION:\n\"" << accused->getConfession() << "\"\n\n"
            << "The stolen Star of Versailles and the confidential ledger are recovered\n"
            << "from behind the secret panel! Monsieur Henri Laurent embraces you in gratitude!\n"
            << BIG_LINE << "\n";
        state = GameState::Won;
    }
    else {
        cout << "\n" << BIG_LINE << "\n"
            << "                         CATASTROPHIC ERROR!                            \n"
            << BIG_LINE << "\n"
            << accused->getName() << " produces an unimpeachable corroborating alibi attested\n"
            << "by three manor guests! The guests gasp at your reckless accusation.\n"
            << accused->getName() << "'s attorney threatens immediate legal sanctions!\n"
            << "Penalty: -20 Credibility, +2 Turns consumed.\n"
            << BIG_LINE << "\n";
        detective->loseCredibility(20);
        detective->useTurns(2);
    }
}
void GameEngine::showDetectiveFile() {
    system("cls");
    detective->display();
    int searchedCount = 0;
    for (int i = 0; i < locations.size(); i++)
        if (locations[i].searched) searchedCount++;
    int interviewedCount = 0;
    for (int i = 0; i < suspects.size(); i++)
        if (suspects[i]->isInterviewed()) interviewedCount++;
    bool canInterviewVictim = detective->getCredibility() >= victim->getRequiredCredibility();
    cout << "\nLocations Searched: " << searchedCount << " / " << locations.size() << "\n"
        << "Suspects Interrogated: " << interviewedCount << " / " << suspects.size() << "\n"
        << "Victim Interview: " << (canInterviewVictim ? "ELIGIBLE" : "LOCKED (<80 CRED)") << "\n"
        << "\nPress ENTER to return...";
    waitForEnter();
}
void GameEngine::forfeitGame() {
    cout << "\nAre you sure you wish to resign from the investigation? (Y/N): ";
    string answer;
    getline(cin, answer);
    answer = toUpperText(trimText(answer));
    if (answer == "Y" || answer == "YES") {
        cout << "\nYou handed over your detective badge. The heist remains unsolved.\n";
        state = GameState::Lost;
    }
}
void GameEngine::checkGameEnd() {
    if (state != GameState::Playing) return;
    if (detective->getCredibility() <= 0) {
        cout << "\n>>> DISMISSAL: Detective credibility has reached 0! You have been stripped of authority! <<<\n";
        state = GameState::Lost;
    }
    else if (detective->getTurnsUsed() >= detective->getMaxTurns()) {
        cout << "\n>>> TIME RUN OUT: 25 turns have expired! The mastermind fled Paris with the diamond! <<<\n";
        state = GameState::Lost;
    }
    else if (word1Solved && word2Solved && !cipherMessageShown) {
        cout << "\n>>> Both cipher words ('" << word1 << "' and '" << word2 << "') have been unlocked on the Scrabble Board! <<<\n"
            << "The vault cipher is cracked! You now have undeniable proof to make your final accusation!\n";
        cipherMessageShown = true;
    }
}
int GameEngine::calculateScore() const {
    if (state == GameState::Won) {
        int turnsLeft = detective->getTurnsRemaining();
        int bonus = (word1Solved ? 100 : 0) + (word2Solved ? 100 : 0);
        return detective->getCredibility() * 10 + turnsLeft * 25 + 500 + bonus;
    }
    return detective->getCredibility() * 2 + (word1Solved ? 50 : 0) + (word2Solved ? 50 : 0);
}
void GameEngine::finishGame() {
    system("cls");
    bool won = (state == GameState::Won);
    int finalScore = calculateScore();
    cout << "\n" << BIG_LINE << "\n"
        << "                              CASE CLOSED                               \n"
        << BIG_LINE << "\n"
        << " Investigation Outcome: " << (won ? "SOLVED (VICTORY)" : "UNSOLVED (DEFEAT)") << "\n"
        << " Detective:             " << detective->getName() << "\n"
        << " Final Credibility:     " << detective->getCredibility() << " / 100\n"
        << " Turns Consumed:        " << detective->getTurnsUsed() << " / " << detective->getMaxTurns() << "\n"
        << " Final Detective Score: " << finalScore << " pts\n";
    if (!won && !suspects.isEmpty()) {
        Suspect* culprit = suspects[culpritIndex];
        cout << " The True Mastermind Culprit was: " << culprit->getName() << " (" << culprit->getRole() << ")!\n"
            << " Reason: " << culprit->getConfession() << "\n";
    }
    cout << BIG_LINE << "\n";
    FileManager::appendScore(LeaderboardEntry(detective->getName(), finalScore, detective->getCredibility(),
        detective->getTurnsUsed(), won ? "WON" : "LOST", currentTimestamp()));
    cout << "\nYour score has been recorded on the Leaderboard.\n"
        << "Press ENTER to return to the Main Menu...";
    waitForEnter();
    state = GameState::Menu;
}
