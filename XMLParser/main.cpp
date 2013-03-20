#include "../engine_main/XMLParser.hpp"

XMLParser p;

int main() {
	p.openFile(std::string
		("C:\\Users\\waco\\Desktop\\engine\\Debug\\test.xml"));
	while(p.gotoNextElement()) {
		std::string n;
		p.readElementName(n);
		std::cout << n << std::endl;
	}
	p.closeFile();

	system("PAUSE");
	return 0;
}