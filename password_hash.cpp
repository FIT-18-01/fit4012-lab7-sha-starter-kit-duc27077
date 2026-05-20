#include "sha256_lib.h"

#include <fstream>
#include <iostream>
#include <string>

namespace {

constexpr const char* DEFAULT_PASSWORD_FILE = "password.hash";

void print_usage(const char* program_name) {
    std::cout << "Usage:\n"
              << "  " << program_name << " register <password_file>\n"
              << "  " << program_name << " verify <password_file>\n"
              << "  " << program_name << " login <password_file>\n"
              << "  " << program_name << " register <password> <password_file>\n"
              << "  " << program_name << " verify <password> <password_file>\n"
              << "  " << program_name << " login <password> <password_file>\n";
}

std::string prompt_password(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
    std::getline(std::cin, password);
    return password;
}

std::string choose_file(int argc, char* argv[]) {
    return (argc == 4) ? std::string(argv[3]) : std::string(argv[2]);
}

bool is_verify_mode(const std::string& mode) {
    return mode == "verify" || mode == "login";
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 3 || argc > 4) {
            print_usage(argv[0]);
            return 1;
        }

        const std::string mode = argv[1];
        const std::string password_file = choose_file(argc, argv);
        const std::string password = (argc == 4)
            ? std::string(argv[2])
            : prompt_password("Enter password: ");

        if (mode == "register") {
            const std::string hash = sha256::calculate_sha256_string(password);
            std::ofstream output(password_file);
            if (!output) {
                throw std::runtime_error("Cannot write password file: " + password_file);
            }
            output << hash << "\n";
            std::cout << "[PASS] Password hash saved to " << password_file << "\n";
            return 0;
        }

        if (is_verify_mode(mode)) {
            std::ifstream input(password_file);
            if (!input) {
                throw std::runtime_error("Cannot read password file: " + password_file);
            }

            std::string stored_hash;
            std::getline(input, stored_hash);
            const std::string current_hash = sha256::calculate_sha256_string(password);

            if (stored_hash == current_hash) {
                std::cout << "[PASS] Password verified\n";
                return 0;
            }

            std::cout << "[FAIL] Password verification failed: wrong password\n";
            return 1;
        }

        print_usage(argv[0]);
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "[ERROR] " << ex.what() << "\n";
        return 1;
    }
}
