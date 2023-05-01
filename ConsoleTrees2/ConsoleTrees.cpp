#include "include.h"


// TODO
// сделать появление дерева при проращивании ростка а не семени 
// наследственность и мутации
// сделать спавн семян при пустом поле
// добавить человеческий рандом
// сделать настраиваемую консоль

template <typename T>
void remove(std::vector<T>& v, size_t index) {
	v.erase(v.begin() + index);
}





enum {
	semen = 0,
	green = 1,
	wood = 2,
	air = 3,
	ground = 4,
};

uint8_t r() {
	return rand() % 15;
}

uint8_t r0() {
	return rand() % 2;
}


// // Пример использования класса PoolContainer
//pc.storage[pc.get_new()] = 12;
//pc.storage[pc.get_new()] = 13;
//pc.storage[pc.get_new()] = 14;
//
//for (int i = 0; i < pc.enabled.size(); i++) {
//	int index = pc.enabled[i];
//	auto& elem = pc.storage[index];
//	if (elem == 14)
//		pc.erase(i);
//}
//
//pc.erase();
//
//for (int i = 0; i < pc.enabled.size(); i++) {
//	int index = pc.enabled[i];
//	auto& elem = pc.storage[index];
//	std::cout << (elem) << std::endl;
//}


template<typename T>
class PoolContainer {
public:
	std::vector<T> storage;

	int get_new() {
		if (disabled.empty()) {
			enabled.push_back(storage.size());
			storage.push_back(T());
		}
		else {
			enabled.push_back(disabled.back());
			disabled.pop_back();
		}
		return enabled.back();
	}

	//int push(T t) {
	//	int tmp = get_new();
	//	storage[tmp] = t;
	//	return tmp;
	//}

	void erase(int index) {
		if (index >= 0 && index < enabled.size()) {
			disabled.push_back(enabled[index]), enabled[index] = -1;
		}
	}

	// убирает мусор из enabled и возвращает в disabled
	void erase() {
		enabled.erase(std::remove_if(enabled.begin(), enabled.end(),
			[&](const int& a) { return a < 0; }
		), enabled.end());
	}

	std::vector<int> enabled;
	std::vector<int> disabled;
};






class Tree {
public:
	int age = 0;
	int energy = 0;
	bool alive = true;
	std::vector<std::vector<uint8_t>> genom = { // что отрастить в стороны + тип клетки
	{r(),r(),r(),r(),semen,},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	{r(),r(),r(),r(),r0(),},
	};
	int size_genom = genom.size();


};



class Cell {
public:

	uint8_t type;
	uint8_t gen_index;
	int index_tree = -1;
	

	Cell(int t) {
		if (t == ground)
			become_ground();
		else
			become_air();
	}


	void become_live(int t, int i_t, int g = 0) {
		type = t;
		index_tree = i_t;
		gen_index = g;
	}

	void become(int t) {
		type = t;
	}

	void become_air() {
		type = air;
	}

	void become_ground() {
		type = ground;
	}

	void become_wood() {
		type = wood;
	}

};






class CellularAutomation {
public:
	PoolContainer<Tree> trees;
	
	//std::vector<Tree> trees;
	std::vector<int> index_live_arr;
	int max_age = 50;
	int width;
	int height, extended_height;
	int length;
	std::vector<Cell> world_map;
	int frame_count = 0;
	enum { up, right, down, left };
	std::vector<int> directions;

	CellularAutomation(int w, int h) {
		
		width = w;
		height = h, extended_height = h + 1;
		length = w * h;
		world_map.resize(width * extended_height, Cell(air));
	
		for (int ind = 0; ind < world_map.size(); ind++) {

			int x = (ind % width);
			int y = (ind / width);

			if (y < 1 || y >= height)
				world_map[ind] = Cell(ground);

		}

		directions = { width , 1, -width, -1 };
	}

	void spawn(int x, int y) {
		try_spawn_semen(x + y * width);
	}

	int get_world_cell_type(int x, int y) {
		return world_map[x + y * width].type;
	}

	void step() {

		// ОСНОВНОЙ ЦИКЛ. ГЛАВНЫЙ. ЦЕНТРАЛЬНЫЙ.
		int old_length = index_live_arr.size();
		for (int ind = 0; ind < old_length; ind++) {
			int index = index_live_arr[ind];
			auto& c = world_map[index];

			if (c.type == semen) {
				// ПОЛУЧИТЬ ИНДЕКС КЛЕТКИ СНИЗУ
				int index_d = index + directions[down];

				switch (world_map[index_d].type) {
				// ПОД СПЕРМОЙ БЛОК ВОЗДУХА. СПЕРМА ПАДАЕТ
				case air:
					index_live_arr[ind] = -index_live_arr[ind];
					c.become_air();
					index_live_arr.push_back(index_d);
					world_map[index_d].become_live(semen, c.index_tree, c.gen_index);
					break;
				// ПОД СПЕРМОЙ ГРУНТ. СПЕРМА ПРОРОСТАЕТ
				case ground:
					c.become(green);
					break;
				// ПОД СПЕРМОЙ НЕПОДХОДЯЩАЯ СРЕДА, СПЕРМА УМИРАЕТ
				default:
					index_live_arr[ind] = -index_live_arr[ind];
					c.become_air();
					//trees.erase(c.index_tree);
				}
			} else {
				// ТЕСТ НА СМЕРТЬ
				if (trees.storage[c.index_tree].alive == false) {
					index_live_arr[ind] = -index_live_arr[ind];
					c.become_air();
					continue;
				}

				// ПОПЫТКА В РАЗМНОЖЕНИЕ
				if (c.type == green) {
					try_grow(c.index_tree, index, c.gen_index);
				}
			}
		}


		// убрать умершие клетки из списка живых
		index_live_arr.erase(std::remove_if(index_live_arr.begin(), index_live_arr.end(),
			// если клетка не живая, в ней будет отрицательный индекс, равный положительному эквиваленту
			[&](const auto& a) { if (a < 0) { return true; } return false; }
		), index_live_arr.end());


		// обработать список деревьев на предмет смерти
		for (int i = 0; i < trees.enabled.size(); i++) {
			int index = trees.enabled[i];
			auto& t = trees.storage[index];
			if (t.age > max_age) {
				t.alive = false;
				trees.erase(i);
			} else 
				t.age++;
		}

		// чисто на всякий случай
		trees.erase();

		// без понятия что делает эта строка ниже
		frame_count++;
	}

private:

	void try_spawn_semen(int ind) {
		if (world_map[ind].type != air)
			return;
		spawn(semen, ind, trees.get_new());
	}



	void spawn(int type, int ind, int ind_tree, int ind_gen = 0) {
		index_live_arr.push_back(ind);
		world_map[ind].become_live(type, ind_tree, ind_gen);
		if (type == semen)
			trees.storage[ind_tree] = Tree();

		
	}

	void try_grow(int index_tree, int index, int gen_index) {
		
	
		auto& gen = trees.storage[index_tree].genom[gen_index];
		// ПО КАЖДОМУ ИЗ НАПРАВЛЕНИЙ
		for (int i = 0; i < 4; i++) {
			// ЕСЛИ ЕСТЬ ЖЕЛАНИЕ РАЗМНОЖИТСЯ
			if (gen[i] < trees.storage[index_tree].size_genom) {
				// ЕСЛИ ЕСТЬ ВОЗМОЖНОСТЬ РАЗМНОЖИТСЯ
				int index_n = index + directions[i];
				if (world_map[index_n].type == air) {
					// РАЗМНОЖИТСЯ
					int type_n =  trees.storage[index_tree].genom[gen[i]][4];
					if (type_n == green) 
						spawn(type_n, index_n, index_tree, gen[i]);
					else if (type_n == semen) 
						spawn(type_n, index_n, trees.get_new(), gen[i]);
				}
			}
		}
		
		//world_map[index].become_wood(); // одеревенение
		//index_live_arr[ind] = -index_live_arr[ind]; // исключение из списка живых
	}


};



int main()
{

	//auto seed = time(NULL);
	//srand(5);
	
	CellularAutomation a(120, 30);
	a.spawn(40, 10);
	
	std::vector<char> str(a.length + 1 , ' ');
	str[a.length] = '\0';
	
	const char gradient[] = ".oO T";
	while (true) {
	
		a.step();
		for (int x = 0; x < 120; x++)
			for (int y = 0; y < 30; y++) {
				str[x + y * 120] = gradient[a.get_world_cell_type(x, 29 - y)];
			}
	
		std::cout << &str[0];
		//Sleep(200);
	}


	return 0;
}