/*Сдвиг влево на 2 бита, буфер 2 байта*/

#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>

bool check_password(std::string password) {
	if (password.empty()) {
		return false;
	}
	if (password.size() > 9) {
		return false;
	}
	for (int i = 0; i < password.size(); i++) {
		if (!('0' <= password[i] && '9' >= password[i])) {
			return false;
		}
	}
	return true;
}

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		std::cerr << "Error: Use three parameters\n";
		return 5;
	}
	const std::string mode(argv[1]); // Режим работы
	const std::string file_name(argv[2]); // Имя файла
	//const std::string mode = "decryption";
	//const std::string mode = "encryption";
	//const std::string file_name = "C:\\Users\\max20\\Desktop\\test.txt";
	if (mode != "encryption" && mode != "decryption") { // Проверка верно ли введён режим работы
		std::cout << "Error: Invalid mode/n";
		return 9;
	}

	std::string password;
	std::cout << "Enter a password (an integer from 0 to 10^9 - 1): ";
	std::getline(std::cin, password); // Вводим пароль и проверяем его на корректность
	if (!check_password(password)) {
		std::cout << "Error: Invalid key/n";
		return 6;
	}
	srand(static_cast<unsigned int>(stoi(password))); // Задаём генирацию псевдослучайных чисел по паролю

	if (mode == "encryption") {
		std::ofstream file(file_name, std::ios_base::trunc | std::ios_base::out); // Открываем файл и проверяем, что он существуют
		if (!file.is_open()) {
			std::cout << "Error: File dosen't exist/n";
			return 1;
		}
		std::string str;
		std::cout << "Enter a text: ";
		std::getline(std::cin, str);
		for (int i = 0; i < str.size(); i += 2) {
			int gamma = rand();
			std::bitset<16> gam(gamma);
			std::bitset<16> bite(str[i]);
			if (i < str.size() - 1) {
				std::bitset<16> bite2(str[i + 1]);
				bite2 = (bite << 8) | bite2;
				bite2 ^= gam;
				bite = bite2;
				bite2 <<= 2;
				if (bite[15]) bite2.set(1);
				if (bite[14]) bite2.set(0);
				file << bite2;
			}
			else {
				bite = (bite << 8) ^ gam;
				std::bitset<8> bite1;
				for (int i = 0; i < 8; i++) if (bite[i + 8]) bite1.set(i);
				bite1 <<= 2;
				if (bite[15]) bite1.set(1);
				if (bite[14]) bite1.set(0);
				file << bite1;
			}
		}
		file.close();
	}
	else if (mode == "decryption") {
		std::ifstream file(file_name, std::ios_base::in); // Открываем файл и проверяем, что он существуют
		if (!file.is_open()) {
			std::cout << "Error: File dosen't exist/n";
			return 1;
		}
		std::string str;
		file >> str;
		std::vector<char> symbs(str.size()/8);
		for (int i = 0; i < str.size(); i += 8) {
			int gamma = rand();
			std::bitset<16> gam(gamma);
			if (i + 8 < str.size()) {
				std::bitset<16> decode_bite(str.substr(i, 16));
				std::bitset<16> midle_result = decode_bite >> 2;
				if (decode_bite[1]) midle_result.set(15);
				if (decode_bite[0]) midle_result.set(14);
				midle_result ^= gam;
				std::bitset<8> first_symb;
				for (int j = 0; j < 8; j++) if (midle_result[j + 8]) first_symb.set(j);
				symbs[i / 8] = (char)first_symb.to_ulong();
				i += 8;
				symbs[i / 8] = (char)midle_result.to_ulong();
			}
			else {
				std::bitset<16> decode_bite(str.substr(i, 8));
				decode_bite <<= 8;
				std::bitset<16> midle_result = decode_bite >> 2;
				if (decode_bite[9]) midle_result.set(15);
				if (decode_bite[8]) midle_result.set(14);
				midle_result ^= gam;
				std::bitset<8> first_symb;
				for (int j = 0; j < 8; j++) if (midle_result[j + 8]) first_symb.set(j);
				symbs[i / 8] = (char)first_symb.to_ulong();
			}
		}
		for (int i = 0; i < symbs.size(); i++) std::cout << symbs[i];
		std::cout << '\n';
		str.clear();
		file.close();
	}
	return 0;
}
