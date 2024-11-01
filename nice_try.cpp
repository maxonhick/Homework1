// Вариант 6 с побитовым сдвигом 3 бита влево и буфером 4 байта. Работает русский язык. Порой вылетают ошибки с ним, но в основной массе работает. 
// Если есть вопросы пишите в тг https://t.me/maxxx412

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
	SetConsoleOutputCP(CP_UTF8); // Работа русского языка
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
	std::cout << "Введите пароль (целое число от 0 до 10^9 - 1): ";
	std::getline(std::cin, password); // Вводим пароль и проверяем его на корректность
	if (!check_password(password)) {
		std::cerr << "Error: Invalid key\n";
		return 6;
	}
	srand(static_cast<unsigned int>(stoi(password))); // Задаём генирацию псевдослучайных чисел по паролю

	if (mode == "encryption") {
		std::string str;
		std::cout << "Введите текст для шифрации: ";
		std::getline(std::cin, str); // Получаем текст для кодировки
		std::vector<std::bitset<8>> lets_code(str.size());
		for (int i = 0; i < str.size(); i++)
			lets_code[i] = std::bitset<8>(str[i]);
		for (int i = 0; i < str.size(); i += 4) {
			int gamma = rand();
			if (i + 3 < str.size()) { // Работаем с полным буфером
				std::bitset<32> gam(gamma); // Преобразуем гамму в 32 битовый набор
				std::bitset<32> bite2; // Объединяем 4 байта в один 32 битовый набор
				for (size_t j = 0; j < 8; j++) {
					bite2[j] = lets_code[i + 3][j];
					bite2[j + 8] = lets_code[i + 2][j];
					bite2[j + 16] = lets_code[i + 1][j];
					bite2[j + 24] = lets_code[i][j];
				}
				bite2 ^= gam; // Гаммируем
				std::bitset<32> res = bite2; // Побитовый сдвиг
				bite2 = bite2 << 2;
				bite2[1] = res[31];
				bite2[0] = res[30];
				for (size_t j = 0; j < 8; j++) { // Запись преобразовынных байтов в массив
					lets_code[i + 3][j] = bite2[j];
					lets_code[i + 2][j] = bite2[j + 8];
					lets_code[i + 1][j] = bite2[j + 16];
					lets_code[i][j] = bite2[j + 24];
				}
			}
			else if (i + 2 < str.size()) { // В буффере 3 байта вместо 4
				std::bitset<24> gam(gamma); // Преобразуем гамму в 24 битовый набор
				std::bitset<24> bite2; // Объединяем 3 байта в один 24 битовый набор
				for (size_t j = 0; j < 8; j++) {
					bite2[j] = lets_code[i + 2][j];
					bite2[j + 8] = lets_code[i + 1][j];
					bite2[j + 16] = lets_code[i][j];
				}
				bite2 ^= gam; // Гаммируем
				std::bitset<24> res = bite2; // Побитовый сдвиг
				bite2 = bite2 << 2;
				bite2[1] = res[23];
				bite2[0] = res[22];
				for (size_t j = 0; j < 8; j++) { // Запись преобразовынных байтов в массив
					lets_code[i + 2][j] = bite2[j];
					lets_code[i + 1][j] = bite2[j + 8];
					lets_code[i][j] = bite2[j + 16];
				}
			}
			else if (i + 1 < str.size()) { // В буфере 2 байта вместо 4
				std::bitset<16> gam(gamma); // Преобразуем гамму в 16 битовый набор
				std::bitset<16> bite2; // Объединяем 2 байта в один 16 битовый набор
				for (size_t j = 0; j < 8; j++) { 
					bite2[j] = lets_code[i + 1][j];
					bite2[j + 8] = lets_code[i][j];
				}
				bite2 ^= gam; // Гаммируем
				std::bitset<16> res = bite2; // Побитовый сдвиг
				bite2 = bite2 << 2;
				bite2[1] = res[15];
				bite2[0] = res[14];
				for (size_t j = 0; j < 8; j++) { // Запись преобразовынных байтов в массив
					lets_code[i + 1][j] = bite2[j];
					lets_code[i][j] = bite2[j + 8];
				}
			}
			else { // Работаем с 1 байтом вместо 4 в буфере
				std::bitset<8> gam(gamma); // Преобразуем гамму в 8 битовый набор
				std::bitset<8> bite2 = lets_code[i];
				bite2 ^= gam; // Гаммируем
				std::bitset<8> res = bite2 << 2; // Побитовый сдвиг
				res[1] = bite2[7];
				res[0] = bite2[6];
				lets_code[i] = res; // Записывемсимволы в массив
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
		for (int i = 0; i < str.size(); i += 4) {
			int gamma = rand();
			if (i + 3 < str.size()) { // Работаем с полным буфером
				std::bitset<32> gam(gamma); // Преобразуем гамму в 32 битовый набор
				std::bitset<32> bite2;
				for (size_t j = 0; j < 8; j++) { // Объединяем 4 байта в один 32 битовый набор
					bite2[j] = code_symb[i + 3][j];
					bite2[j + 8] = code_symb[i + 2][j];
					bite2[j + 16] = code_symb[i + 1][j];
					bite2[j + 24] = code_symb[i][j];
				}
				std::bitset<32> res = bite2;
				bite2 = bite2 >> 2; // Побитовый сдвиг
				bite2[31] = res[1];
				bite2[30] = res[0];
				bite2 ^= gam; // Гаммируем
				std::bitset<8> fs, ss, ts, fus;
				for (size_t j = 0; j < 8; j++) { // Разделяем расшифрованные байты
					fus[j] = bite2[j];
					ts[j] = bite2[j + 8];
					ss[j] = bite2[j + 16];
					fs[j] = bite2[j + 24];
				}
				str[i] = static_cast<char>(fs.to_ulong()); // Записывемсимволы в массив
				str[i + 1] = static_cast<char>(ss.to_ulong());
				str[i + 2] = static_cast<char>(ts.to_ulong());
				str[i + 3] = static_cast<char>(fus.to_ulong());
			}
			else if (i + 2 < str.size()) {
				std::bitset<24> gam(gamma); // Преобразуем гамму в 24 битовый набор
				std::bitset<24> bite2;
				for (size_t j = 0; j < 8; j++) { // Объединяем 3 байта в один 24 битовый набор
					bite2[j] = code_symb[i + 2][j];
					bite2[j + 8] = code_symb[i + 1][j];
					bite2[j + 16] = code_symb[i][j];
				}
				std::bitset<24> res = bite2; // Побитовый сдвиг
				bite2 = bite2 >> 2;
				bite2[23] = res[1];
				bite2[22] = res[0];
				bite2 ^= gam; // Гаммируем
				std::bitset<8> fs, ss, ts;
				for (size_t j = 0; j < 8; j++) { // Разделяем расшифрованные байты
					ts[j] = bite2[j];
					ss[j] = bite2[j + 8];
					fs[j] = bite2[j + 16];
				}
				str[i] = static_cast<char>(fs.to_ulong()); // Записывемсимволы в массив
				str[i + 1] = static_cast<char>(ss.to_ulong());
				str[i + 2] = static_cast<char>(ts.to_ulong());
			}
			else if (i + 1 < str.size()) {
				std::bitset<16> gam(gamma); // Преобразуем гамму в 16 битовый набор
				std::bitset<16> bite2;
				for (size_t j = 0; j < 8; j++) { // Объединяем 2 байта в один 16 битовый набор
					bite2[j] = code_symb[i + 1][j];
					bite2[j + 8] = code_symb[i][j];
				}
				std::bitset<16> res = bite2;
				bite2 = bite2 >> 2; // Побитовый сдвиг
				bite2[15] = res[1];
				bite2[14] = res[0];
				bite2 ^= gam; // Гаммируем
				std::bitset<8> fs, ss;
				for (size_t j = 0; j < 8; j++) { // Разделяем расшифрованные байты
					ss[j] = bite2[j];
					fs[j] = bite2[j + 8];
				}
				str[i] = static_cast<char>(fs.to_ulong()); // Записывемсимволы в массив
				str[i + 1] = static_cast<char>(ss.to_ulong());
			}
			else { // Работаем с не полным буффером
				std::bitset<8> gam(gamma); // Преобразуем гамму в 8 битовый набор
				std::bitset<8> bite2 = code_symb[i];
				bite2 = bite2 >> 2; // Побитовый сдвиг
				bite2[7] = code_symb[i][1];
				bite2[6] = code_symb[i][0];
				bite2 ^= gam; // Гаммируем
				str[i] = static_cast<char>(bite2.to_ulong()); // Записывемсимволы в массив
			}
		}
		for (int i = 0; i < str.size(); i++) 
			std::cout << str[i]; // Вывод символов конечного массива
		std::cout << '\n';
		str.clear();
	}
	return 0;
}
