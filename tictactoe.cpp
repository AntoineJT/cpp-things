#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>

enum Player {
    None,
    Circle,
    Cross
};

enum GameState {
    CircleWinned,
    CrossWinned,
    OnGoing,
    Equality
};

// I read somewhere that this is more idiomatic than typedefs
using Board = std::array<Player, 9>;
using Line = std::array<std::size_t, 3>;

char GetPlayerSymbol(Player player) noexcept {
    switch (player) {
        case None: return ' ';
        case Circle: return 'O';
        case Cross: return 'X';
    }
}

constexpr std::array<Line, 3> hlines {{ 
    {0, 1, 2}, 
    {3, 4, 5}, 
    {6, 7, 8} 
}};
constexpr std::array<Line, 3> vlines {{ 
    {0, 3, 6}, 
    {1, 4, 7}, 
    {2, 5, 8} 
}};
constexpr std::array<Line, 2> dlines {{ 
    {0, 4, 8}, 
    {2, 4, 6} 
}};

struct Game {
    Game()
     : m_board(Board { None })
     , m_currentPlayer(Player::Cross)
     , m_gameState(GameState::OnGoing)
     {}

    private:
        Board m_board;
        Player m_currentPlayer;
        GameState m_gameState;

        bool IsLineOwned(Line line) const noexcept {
            #ifndef NDEBUG
            for (auto index : line) {
                assert(index < 9);
            }
            #endif

            const auto middleCellOwner = m_board[line[1]];
            return middleCellOwner == m_board[line[0]]
                && middleCellOwner == m_board[line[2]]
                && middleCellOwner != Player::None;
        }

        GameState UpdateGameState(Player winner) const noexcept {
            switch (winner) {
                case Cross: return GameState::CrossWinned;
                case Circle: return GameState::CircleWinned;
                case None: return GameState::OnGoing;
            }
        }

    public:
        void SwitchToOpponent() noexcept {
            assert(m_currentPlayer != None);
            
            switch (m_currentPlayer) {
                case Cross:
                    m_currentPlayer = Player::Circle;
                    break;
                case Circle:
                    m_currentPlayer = Player::Cross;
                    break;
                default:
                    break;
            }
        }

        void PrintBoard() const noexcept {
            std::array<char, 9> symbols {};
            for (auto i = 0; i < 9; ++i) {
                symbols[i] = GetPlayerSymbol(m_board[i]);
            }

            std::cout 
                << "     |     |     " << std::endl
                << "  " << symbols[0] << "  |  " << symbols[1] << "  |  " << symbols[2] << std::endl
                << "_____|_____|_____" << std::endl
                << "     |     |     " << std::endl
                << "  " << symbols[3] << "  |  " << symbols[4] << "  |  " << symbols[5] << std::endl
                << "_____|_____|_____" << std::endl
                << "     |     |     " << std::endl
                << "  " << symbols[6] << "  |  " << symbols[7] << "  |  " << symbols[8] << std::endl
                << "     |     |     " << std::endl 
                << std::endl;
        }

        bool InProgress() const noexcept {
            return m_gameState == OnGoing;
        }

        std::string WinnerName() const noexcept {
            assert(!InProgress() && m_gameState != Equality);
            
            switch (m_gameState) {
                case CircleWinned: return "Circle";
                case CrossWinned: return "Cross";
                case Equality:
                case OnGoing:
                default:
                    return "Invalid";
            }
        }

        std::string CurrentPlayerName() const noexcept {
            assert(m_currentPlayer != None);

            switch (m_currentPlayer) {
                case Cross: return "Cross";
                case Circle: return "Circle";
                case None:
                default:
                    return "Invalid";
            }
        }

        bool CanBePlayed(std::size_t cellIndex) const noexcept {
            assert(cellIndex < 9);

            return m_board[cellIndex] == None;
        }

        void PlayCell(std::size_t cellIndex) noexcept {
            assert(cellIndex < 9);

            m_board[cellIndex] = m_currentPlayer;
        }

        void UpdateGameState() noexcept {
            assert(m_gameState == OnGoing);
            
            // TODO Find a way to avoid to do some code repetition
            for (auto line : hlines) {
                if (IsLineOwned(line)) {
                    m_gameState = UpdateGameState(m_board[line[0]]);
                    return;
                }
            }
            for (auto line : vlines) {
                if (IsLineOwned(line)) {
                    m_gameState = UpdateGameState(m_board[line[0]]);
                    return;
                }
            }
            for (auto line : dlines) {
                if (IsLineOwned(line)) {
                    m_gameState = UpdateGameState(m_board[line[0]]);
                    return;
                }
            }

            bool equality = true;
            for (auto cellOwner : m_board) {
                if (cellOwner == None) {
                    equality = false;
                    break;
                }
            }
            if (equality) {
                m_gameState = GameState::Equality;
            }
        }

        GameState State() const noexcept {
            return m_gameState;
        }
};

int main() {
    std::cout 
        << "-------------" << std::endl
        << " Tic Tac Toe" << std::endl
        << "-------------" << std::endl
        << std::endl;

    Game game = Game();

    std::cout 
        << "Press 0 to quit." << std::endl
        << "Press 1 to 9 to play a cell." << std::endl
        << std::endl;

    while (game.InProgress()) {
        game.PrintBoard();

        int input;
        std::cout
            << "It's the turn of the '" << game.CurrentPlayerName() << "' player." << std::endl
            << "Play a cell: ";
        std::cin >> input;

        if (std::cin.eof()) {
            throw std::runtime_error("Input stream has been closed!");
        } else
        if (std::cin.fail()) {
            std::cout << "Invalid input. Try again!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            if (input < 0 || input > 9) {
                std::cout << "Number out of range: 0 to quit, 1-9 to play a cell." << std::endl;
            } else {
                if (input == 0) {
                    exit(EXIT_SUCCESS);
                }
                --input; // array bounds are from 0 to 8, input is from 1 to 9
                if (!game.CanBePlayed(input)) {
                    std::cout << "This cell is already owned! Play another one." << std::endl;
                } else {
                    game.PlayCell(input);
                    game.UpdateGameState();
                    if (!game.InProgress()) {
                        game.PrintBoard();
                        if (game.State() == Equality) {
                            std::cout << "Equality! No one wins." << std::endl;
                        } else {
                            std::cout << "Player '" << game.WinnerName() << "' won!" << std::endl;
                        }
                    } else {
                        game.SwitchToOpponent();
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
