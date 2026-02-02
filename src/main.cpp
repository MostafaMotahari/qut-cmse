#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#else
#include <unistd.h>
#endif

// Function to clear screen (cross-platform)
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to print CMSE ASCII art
void printCMSEHeader() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                                ║
║  ██████╗███╗   ███╗███████╗███████╗                                           ║
║ ██╔════╝████╗ ████║██╔════╝██╔════╝                                           ║
║ ██║     ██╔████╔██║█████╗  ███████╗                                           ║
║ ██║     ██║╚██╔╝██║██╔══╝  ╚════██║                                           ║
║ ╚██████╗██║ ╚═╝ ██║███████╗███████║                                           ║
║  ╚═════╝╚═╝     ╚═╝╚══════╝╚══════╝                                           ║
║                                                                                ║
║           ╔══════════════════════════════════════════════════════════╗         ║
║           ║      Columnar Multi-model Storage Engine v1.0.0          ║         ║
║           ╚══════════════════════════════════════════════════════════╝         ║
║                                                                                ║
╚════════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;
}

// Function to print a colorful version (if terminal supports ANSI codes)
void printColorfulCMSE() {
    std::cout << "\033[1;36m";  // Cyan bold
    
    std::cout << R"(
  ╔══════════════════════════════════════════════════════════════════════╗
  ║                                                                      ║
  ║  ██████╗███╗   ███╗███████╗███████╗                                 ║
  ║ ██╔════╝████╗ ████║██╔════╝██╔════╝                                 ║
  ║ ██║     ██╔████╔██║█████╗  ███████╗                                 ║
  ║ ██║     ██║╚██╔╝██║██╔══╝  ╚════██║                                 ║
  ║ ╚██████╗██║ ╚═╝ ██║███████╗███████║                                 ║
  ║  ╚═════╝╚═╝     ╚═╝╚══════╝╚══════╝                                 ║
  ║                                                                      ║
)" << "\033[1;33m" << R"(  ║          ╔════════════════════════════════════════════════╗          ║
  ║          ║     Columnar Multi-model Storage Engine      ║          ║
  ║          ╚════════════════════════════════════════════════╝          ║
  ║                                                                      ║
)" << "\033[1;35m" << R"(  ║          Version: 1.0.0 | Build: " << __DATE__ << " " << __TIME__ << R"(          ║
  ║                                                                      ║
  ╚══════════════════════════════════════════════════════════════════════╝
)" << "\033[0m" << std::endl;
}

// Alternative minimalist ASCII art
void printMinimalCMSE() {
    std::cout << R"(
┌────────────────────────────────────────────────────────────────────────┐
│                                                                        │
│    ██████╗ ███╗   ███╗███████╗███████╗                                │
│   ██╔════╝ ████╗ ████║██╔════╝██╔════╝                                │
│   ██║  ███╗██╔████╔██║█████╗  ███████╗                                │
│   ██║   ██║██║╚██╔╝██║██╔══╝  ╚════██║                                │
│   ╚██████╔╝██║ ╚═╝ ██║███████╗███████║                                │
│    ╚═════╝ ╚═╝     ╚═╝╚══════╝╚══════╝                                │
│                                                                        │
│              ┌────────────────────────────────────────┐                │
│              │   CMSE Database Management System      │                │
│              └────────────────────────────────────────┘                │
│                                                                        │
└────────────────────────────────────────────────────────────────────────┘
)" << std::endl;
}

// Function to display menu
void displayMenu() {
    std::cout << "\n\033[1;32m" << "┌───────────────────────── Main Menu ─────────────────────────┐" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "                                                              " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  1. Create Index                                    " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  2. Insert Data                                      " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  3. Search Data                                      " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  4. Execute Query                                    " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  5. View Statistics                                  " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  6. Benchmark                                        " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  7. System Status                                    " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "  8. Exit                                             " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "│" << "\033[0m" << "                                                              " << "\033[1;32m" << "│" << "\033[0m" << std::endl;
    std::cout << "\033[1;32m" << "└──────────────────────────────────────────────────────────────┘" << "\033[0m" << std::endl;
    std::cout << "\n\033[1;36m" << "CMSE> " << "\033[0m";
}

// Function to simulate loading animation
void showLoading() {
    std::cout << "\033[1;33m" << "\nInitializing CMSE Database Engine" << "\033[0m" << std::endl;
    
    for (int i = 0; i < 3; i++) {
        std::cout << "\033[1;33m" << "." << "\033[0m" << std::flush;
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500000);
#endif
    }
    
    std::cout << "\033[1;32m" << " READY!" << "\033[0m" << std::endl;
#ifdef _WIN32
    Sleep(300);
#else
    usleep(300000);
#endif
}

// Main interactive loop
void runInteractive() {
    std::string input;
    
    while (true) {
        displayMenu();
        std::getline(std::cin, input);
        
        if (input.empty()) {
            continue;
        }
        
        // Convert to lowercase for easier comparison
        std::string cmd = input;
        for (char &c : cmd) c = std::tolower(c);
        
        if (cmd == "1" || cmd == "create" || cmd == "create index") {
            std::cout << "\033[1;34m" << "[CREATE INDEX] Enter index type (btree/trie): " << "\033[0m";
            std::getline(std::cin, input);
            std::cout << "\033[1;32m" << "Creating " << input << " index..." << "\033[0m" << std::endl;
        }
        else if (cmd == "2" || cmd == "insert" || cmd == "insert data") {
            std::cout << "\033[1;34m" << "[INSERT] Enter data to insert: " << "\033[0m";
            std::getline(std::cin, input);
            std::cout << "\033[1;32m" << "Inserted: '" << input << "'" << "\033[0m" << std::endl;
        }
        else if (cmd == "3" || cmd == "search" || cmd == "search data") {
            std::cout << "\033[1;34m" << "[SEARCH] Enter search term: " << "\033[0m";
            std::getline(std::cin, input);
            std::cout << "\033[1;32m" << "Searching for: '" << input << "'" << "\033[0m" << std::endl;
        }
        else if (cmd == "4" || cmd == "query" || cmd == "execute query") {
            std::cout << "\033[1;34m" << "[QUERY] Enter SQL query: " << "\033[0m";
            std::getline(std::cin, input);
            std::cout << "\033[1;32m" << "Executing query..." << "\033[0m" << std::endl;
        }
        else if (cmd == "5" || cmd == "stats" || cmd == "statistics") {
            std::cout << "\033[1;34m" << "[STATISTICS] Generating report..." << "\033[0m" << std::endl;
            std::cout << "\033[1;33m" << "Indexes: 2" << std::endl;
            std::cout << "Records: 15,432" << std::endl;
            std::cout << "Memory: 128 MB" << "\033[0m" << std::endl;
        }
        else if (cmd == "6" || cmd == "benchmark") {
            std::cout << "\033[1;34m" << "[BENCHMARK] Running performance tests..." << "\033[0m" << std::endl;
            std::cout << "\033[1;33m" << "Insert speed: 12,500 ops/sec" << std::endl;
            std::cout << "Query speed: 8,200 ops/sec" << "\033[0m" << std::endl;
        }
        else if (cmd == "7" || cmd == "status" || cmd == "system status") {
            std::cout << "\033[1;34m" << "[SYSTEM STATUS]" << "\033[0m" << std::endl;
            std::cout << "\033[1;33m" << "Buffer Pool: 50 pages" << std::endl;
            std::cout << "Disk Usage: 45 MB" << std::endl;
            std::cout << "Uptime: 00:15:32" << "\033[0m" << std::endl;
        }
        else if (cmd == "8" || cmd == "exit" || cmd == "quit" || cmd == "q") {
            std::cout << "\033[1;31m" << "\nAre you sure you want to exit? (y/n): " << "\033[0m";
            std::getline(std::cin, input);
            if (input == "y" || input == "Y" || input == "yes") {
                std::cout << "\033[1;32m" << "\nShutting down CMSE Database Engine..." << "\033[0m" << std::endl;
                std::cout << "\033[1;33m" << "Goodbye!" << "\033[0m" << std::endl;
                break;
            }
        }
        else if (cmd == "clear" || cmd == "cls") {
            clearScreen();
            printColorfulCMSE();
        }
        else if (cmd == "help") {
            std::cout << "\033[1;34m" << "[HELP] Available commands:" << "\033[0m" << std::endl;
            std::cout << "  Menu options 1-8" << std::endl;
            std::cout << "  clear/cls - Clear screen" << std::endl;
            std::cout << "  help - Show this help" << std::endl;
            std::cout << "  exit/quit - Exit program" << std::endl;
        }
        else {
            std::cout << "\033[1;31m" << "Unknown command: " << cmd << "\033[0m" << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
        
        std::cout << std::endl;
    }
}

int main() {
    // Clear screen
    clearScreen();
    
    // Print CMSE ASCII art
    printColorfulCMSE();
    
    // Show loading animation
    showLoading();
    
    // Run interactive mode
    runInteractive();
    
    return 0;
}
