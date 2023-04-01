#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Minterm {
 public:
	int value;  // The value of the minterm
	bool isCovered;  // A flag to indicate if the minterm is used in a prime implicant
	Minterm() {
		value = 0;
		isCovered = false;
	}
};

class Implicant {
 public:
	vector<Minterm*> minterms;  // The minterms covered by the prime implicant
	vector<int> binary;    // The binary representation of the prime implicant
	bool isUsed;// A flag to indicate if the prime implicant is used in a minimum cover
	Implicant() {
		isUsed = false;
	}
	void print() const {
		cout << "m(";
		for (auto minterm : minterms) {
			cout << minterm->value;
			if (minterm != minterms.back()) {
				cout << ", ";
			}
		}
		cout << ") = ";
		for (int i = 0; i < binary.size(); i++) {
			if (binary[i] == 0) {
				cout << char('A' + i) << "'";
			} else if (binary[i] == 1) {
				cout << char('A' + i);
			}
		}
		cout << endl;
	}
};

// A function to calculate the binary representation of a minterm
vector<int> getBinary(int minterm, int numVars) {
	vector<int> binary(numVars, 0);
	for (int i = numVars - 1; i >= 0; i--) {
		binary[i] = minterm % 2;
		minterm /= 2;
	}
	return binary;
}

#ifdef OPTIMIZE
// A function to calculate the number of ones in a binary vector
int countOnes(const vector<int> &binary) {
	int count = 0;
	for (int i : binary) {
		if (i == 1) {
			count++;
		}
	}
	return count;
}
#endif

// A function to check if two binary vectors differ by only one bit
bool isDifferByOne(vector<int> binary1, vector<int> binary2) {
	int count = 0;
	for (int i = 0; i < binary1.size(); i++) {
		if (binary1[i] != binary2[i]) {
			count++;
		}
		if (count > 1) {
			return false;
		}
	}
	return count == 1;
}

class QuineMcCluskey {
	//TODO: Separate the column by the number of ones in the binary representation
 	//TODO: Process the don't care terms
 public:
	vector<Minterm> minterms;
	int numVars{};
	vector<vector<Implicant>> columns;
	vector<Implicant> essentialImplicants;
	vector<Implicant> primeImplicants;
	QuineMcCluskey() = default;
	
	void readData(){
		cout << "Enter the number of variables:";
		cin >> numVars;
		columns.resize(numVars + 1);
		cout << "Enter the minterms separated by spaces (use -1 to terminate input):";
		int mintermValue;
		while (cin >> mintermValue) {
			if (mintermValue == -1) {
				break;
			}
			Minterm newMinterm;
			newMinterm.value = mintermValue;
			minterms.push_back(newMinterm);
		}
	}
	
	static bool isInColumn(const vector<Implicant> &column, const Implicant &implicant) {
		for (auto &primeImplicant : column) {
			if (primeImplicant.binary == implicant.binary) {
				return true;
			}
		}
		return false;
	}
	
	void printResult() const {
		cout << "Prime Implicants: " << endl;
		for (auto &primeImplicant : primeImplicants) {
			primeImplicant.print();
		}
		
		cout << endl << "Essential Prime Implicants: " << endl;
		for (auto &essentialImplicant : essentialImplicants) {
			essentialImplicant.print();
		}
	}
	
	void initializeFirstColumnOfColumns() {
		for (auto &minterm : minterms) {
			Implicant newImplicant;
			newImplicant.minterms.push_back(&minterm);
			newImplicant.binary = getBinary(minterm.value, numVars);
			columns[0].push_back(newImplicant);
		}
	}
	
	void generateRestOfColumns() {
		for (int i = 0; i < numVars; i++) {
			for (int j = 0; j < columns[i].size(); j++) {
				for (int k = j + 1; k < columns[i].size(); k++) {
					Implicant &lastImplicant1 = columns[i][j];
					Implicant &lastImplicant2 = columns[i][k];
					
					if (isDifferByOne(lastImplicant1.binary, 
									  lastImplicant2.binary)) {
						/*minterm1.print();
						minterm2.print();*/
						lastImplicant1.isUsed = true;
						lastImplicant2.isUsed = true;
						Implicant newImplicant;
						newImplicant.binary = lastImplicant1.binary;//also equals to minterm2.binary
						for (int l = 0; l < newImplicant.binary.size(); l++) {
							if (newImplicant.binary[l] != lastImplicant2.binary[l]) {
								newImplicant.binary[l] = -1;
								break;
							}
						}
						newImplicant.minterms.insert(newImplicant.minterms.end(),
												  lastImplicant1.minterms.begin(),
												  lastImplicant1.minterms.end());
						newImplicant.minterms.insert(newImplicant.minterms.end(),
												  lastImplicant2.minterms.begin(),
												  lastImplicant2.minterms.end());
						if (!isInColumn(columns[i+1], newImplicant)) {
							columns[i+1].push_back(newImplicant);
						}
						
					}
				}
			}
		}
	}
	
	void generatePrimeImplicants() {
		for (const auto &column : columns) {
			for (auto &minterm : column) {
				if (!minterm.isUsed) {
					primeImplicants.push_back(minterm);
				}
			}
		}
	}
	
	bool isMintermCoveredJustOnce(const Minterm &minterm) {
		int count = 0;
		for (auto primeImplicant : primeImplicants) {
			if (find(primeImplicant.minterms.begin(),
					 primeImplicant.minterms.end(), &minterm)
				!= primeImplicant.minterms.end()) {
				count++;
			}
		}
		return count == 1;
	}
	
	void initializeEssentialPrimeImplicants() {
		for (auto &minterm : minterms) {
			if (isMintermCoveredJustOnce(minterm)) {
				for(auto &primeImplicant : primeImplicants) {
					if (find(primeImplicant.minterms.begin(),
							 primeImplicant.minterms.end(), &minterm)
						!= primeImplicant.minterms.end()) {
						if(!primeImplicant.isUsed) {
							primeImplicant.isUsed = true;
							essentialImplicants.push_back(primeImplicant);
						}
						break;
					}
				}
			}
		}
	}
	
	// Remove the covered minterms from essential prime implicants
	void removeCoveredMinterms() {
		for (auto &essentialImplicant : essentialImplicants) {
			for (auto minterm : essentialImplicant.minterms) {
				minterm->isCovered = true;
			}
		}
	}
	
	void extractEssentialPrimeImplicants() {
		
		initializeEssentialPrimeImplicants();
		while(true){
			removeCoveredMinterms();
			int MaxNumCoveredMinterms = 0;
			Implicant *newEssentialImplicant;
			for (auto &primeImplicant : primeImplicants) {
				if (!primeImplicant.isUsed) {
					int numCoveredMinterms = 0;
					for (auto minterm : primeImplicant.minterms) {
						if (!minterm->isCovered) {
							numCoveredMinterms++;
						}
					}
					if(numCoveredMinterms > MaxNumCoveredMinterms) {
						MaxNumCoveredMinterms = numCoveredMinterms;
						newEssentialImplicant = &primeImplicant;
					}
				}
			}
			if (!MaxNumCoveredMinterms) {
				break;
			}
			newEssentialImplicant->isUsed = true;
			essentialImplicants.push_back(*newEssentialImplicant);
		}
	}
	
	// A function to find all prime implicants from a list of minterms
	void runAlgorithm() {
		
		readData();
		initializeFirstColumnOfColumns();
		generateRestOfColumns();
		generatePrimeImplicants();
		extractEssentialPrimeImplicants();
		printResult();
	}
};

int main() {
	QuineMcCluskey quineMcCluskey;
	quineMcCluskey.runAlgorithm();
	return 0;
}