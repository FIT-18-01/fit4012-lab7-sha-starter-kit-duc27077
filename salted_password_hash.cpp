#include "sha256_lib.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr const char* DEFAULT_PASSWORD_FILE = "password.hash";

void print_usage(const char* program_name) {
    std::cout << "Usage:\n"
              << "  " << program_name << " register <password_file>\n"
              << "  " << program_name << " verify <password_file>\n"
              << "  " << program_name << " login <password_file>\n"
              << "  " << program_name << " register <password> <password_file>\n"
              << "  " << program_name << " verify <password> <password_file>\n"
              << "  " << program_name << " login <password> <password_file>\n"
              << "  " << program_name << " hash-with-salt <password> <salt_hex>\n";
}

std::string bytes_to_hex(const std::vector<byte>& bytes) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (byte b : bytes) {
        oss << std::setw(2) << static_cast<int>(b);
    }
    return oss.str();
}

std::string random_salt_hex(std::size_t byte_count = 16) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 255);

    std::vector<byte> salt(byte_count);
    for (byte& b : salt) {
        b = static_cast<byte>(dist(rd));
    }
    return bytes_to_hex(salt);
}

std::string normalize_hex(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

std::string salted_hash(const std::string& salt_hex, const std::string& password) {
    // Teaching demo: hash(salt_hex + password).
    // In production, use a password-hashing algorithm such as Argon2id, bcrypt or scrypt.
    return sha256::calculate_sha256_string(normalize_hex(salt_hex) + password);
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

        if (mode == "hash-with-salt") {
            if (argc != 4) {
                print_usage(argv[0]);
                return 1;
            }
            const std::string salt_hex = argv[3];
            std::cout << salted_hash(salt_hex, password) << "\n";
            return 0;
        }

        if (mode == "register") {
            const std::string salt_hex = random_salt_hex();
            const std::string hash = salted_hash(salt_hex, password);

            std::ofstream output(password_file);
            if (!output) {
                throw std::runtime_error("Cannot write password file: " + password_file);
            }

            output << salt_hex << ":" << hash << "\n";
            std::cout << "[PASS] Salted password hash saved to " << password_file << "\n";
            return 0;
        }

        if (is_verify_mode(mode)) {
            std::ifstream input(password_file);
            if (!input) {
                throw std::runtime_error("Cannot read password file: " + password_file);
            }

            std::string line;
            std::getline(input, line);
            const std::size_t sep = line.find(':');
            if (sep == std::string::npos) {
                throw std::runtime_error("Invalid password file format. Expected salt:hash");
            }

            const std::string salt_hex = line.substr(0, sep);
            const std::string stored_hash = line.substr(sep + 1);
            const std::string current_hash = salted_hash(salt_hex, password);

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
