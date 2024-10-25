/*Сдвиг влево на 2 бита, буфер 2 байта*/

#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>
#include <Windows.h>

bool check_password(std::string password) { // проверка пароля
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

void splitBitset(const std::bitset<16>& combined, std::bitset<8>& high, std::bitset<8>& low) { // Разделение 16 битового набора на два 8 битовых набора
	for (size_t i = 0; i < 8; ++i) {
		low[i] = combined[i];
		high[i] = combined[i + 8];
	}
}

void writeBinaryToFile(const std::string& filename, const std::vector<std::bitset<8>>& data) { //Запись 8 битовых наборов в файл
	std::ofstream outFile(filename, std::ios::binary);
	if (!outFile) {
		std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
		return;
	}
	for (const auto& bitset : data) {
		unsigned char c = static_cast<unsigned char>(bitset.to_ulong());
		outFile.write(reinterpret_cast<const char*>(&c), sizeof(c));
	}
	outFile.close();
}

std::bitset<16> combineBitsets(const std::bitset<8>& high, const std::bitset<8>& low) { // Обхединение двух bitset на 8 бит в один на 16
	std::bitset<16> combined;
	for (size_t i = 0; i < 8; ++i) {
		combined[i] = low[i];
		combined[i + 8] = high[i];
	}
	return combined;
}

// Функция для чтения всех символов из текстового файла, даже перехода на следующую строку
std::vector<char> readAllCharactersFromFile(const std::string& filename) {
	std::ifstream inFile(filename);
	if (!inFile) {
		std::cerr << "Не удалось открыть файл для чтения: " << filename << std::endl;
		return {};
	}
	std::vector<char> data;
	char c;
	while (inFile.get(c)) {
		data.push_back(c);
	}
	inFile.close();
	return data;
}

int main(int argc, const char* argv[]) {
	SetConsoleOutputCP(CP_UTF8); // Работа русского языка (хотя работает только с выводом, но не с кодировкой)
	SetConsoleCP(CP_UTF8);
	if (argc != 3) {
		std::cerr << "Error: Use three parameters\n";
		return 5;
	}
	const std::string mode(argv[1]); // Режим работы
	const std::string file_name(argv[2]); // Имя файла
	if (mode != "encryption" && mode != "decryption") { // Проверка верно ли введён режим работы
		std::cerr << "Error: Invalid mode\n";
		return 9;
	}

	std::string password;
	std::cout << "Enter a password (an integer from 0 to 10^9 - 1): ";
	std::getline(std::cin, password); // Вводим пароль и проверяем его на корректность
	if (!check_password(password)) {
		std::cerr << "Error: Invalid key\n";
		return 6;
	}
	srand(static_cast<unsigned int>(stoi(password))); // Задаём генирацию псевдослучайных чисел по паролю

	if (mode == "encryption") {
		std::string str;
		std::cout << "Enter a text: ";
		std::getline(std::cin, str); // Получаем текст для кодировки
		std::vector<std::bitset<8>> lets_code(str.size());
		for (int i = 0; i < str.size(); i += 2) {
			int gamma = rand();
			std::bitset<16> gam(gamma); // Преобразуем гамму в 16 битовый набор
			std::bitset<16> bite(str[i]);
			if (i < str.size() - 1) { // Работаем с полным буфером
				std::bitset<16> bite2(str[i + 1]);
				bite2 = (bite << 8) | bite2;
				bite2 ^= gam; // Гаммируем
				bite = bite2; // Побитовый сдвиг
				bite2 <<= 2;
				if (bite[15]) bite2.set(1);
				if (bite[14]) bite2.set(0);
				std::bitset<8> fs, ss;
				splitBitset(bite2, fs, ss); // Разделяем буффер на отдельные символы
				lets_code[i] = fs; // Записывемсимволы в массив
				lets_code[i + 1] = ss;
			}
			else { // Работаем с не полным буффером
				bite = (bite << 8) ^ gam; // Гаммируем
				std::bitset<8> bite1;
				for (int i = 0; i < 8; i++) 
					if (bite[i + 8]) 
						bite1.set(i);
				bite1 <<= 2; // Побитовый сдвиг
				if (bite[15]) bite1.set(1);
				if (bite[14]) bite1.set(0);
				lets_code[i] = bite1; // Записываем симвл в массив
			}
		}
		writeBinaryToFile(file_name, lets_code); // Записываем все символы в файл
	}
	else if (mode == "decryption") { // Расшифровываем
		std::vector<char> str = readAllCharactersFromFile(file_name); // Считываем все символы в массив
		std::vector<std::bitset<8>> code_symb(str.size());
		for (int i = 0; i < str.size(); i++) { // Преобразуем все символы в 8 битовое представление
			code_symb[i] = std::bitset<8>(str[i]);
		}
		for (int i = 0; i < str.size(); i += 2) {
			int gamma = rand();
			std::bitset<16> gam(gamma);
			if (i < str.size()) { // работаем с полным буфером
				std::bitset<16> decode_bite = combineBitsets(code_symb[i], code_symb[i + 1]); // Объединяем два 8 битовых массива в один 16 битовый
				std::bitset<16> midle_result = decode_bite >> 2; // Побитовый сдвиг
				if (decode_bite[1]) midle_result.set(15);
				if (decode_bite[0]) midle_result.set(14);
				midle_result ^= gam; // Гаммирование
				std::bitset<8> fs, ss;
				splitBitset(midle_result, fs, ss); // Разделение массива на отдельные символы
				str[i] = static_cast<char>(fs.to_ulong()); // Запись символов в итоговый массив
				str[i + 1] = static_cast<char>(ss.to_ulong());
			}
			else { // Не полный буффер
				std::bitset<16> decode_bite = combineBitsets(code_symb[i], std::bitset<8>(0));
				std::bitset<16> midle_result = decode_bite >> 2; // Побитовый сдвиг
				if (decode_bite[1]) midle_result.set(15);
				if (decode_bite[0]) midle_result.set(14);
				midle_result ^= gam; // Гаммирование
				std::bitset<8> fs, ss;
				splitBitset(midle_result, fs, ss);
				str[i] = static_cast<char>(fs.to_ulong()); // Запись в массив
			}
		}
		for (int i = 0; i < str.size(); i++) 
			std::cout << str[i]; // Вывод символов конечного массива
		std::cout << '\n';
		str.clear();
	}
	return 0;
}
