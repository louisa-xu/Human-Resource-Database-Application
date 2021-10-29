#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

const int MIN_OPTION = 0;
const int MAX_OPTION = 5;
const char* DEFAULT_OFFICECODE = "1";
const int DEFAULT_MGR = 1002;

struct Employee
{
	int  employeeNumber;
	char lastName[50];
	char firstName[50];
	char extension[10];
	char email[100];
	char officecode[10];
	int  reportsTo;
	char jobTitle[50];
};

int menu(void);
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp);
void displayEmployee(Connection* conn, struct Employee emp);
void displayAllEmployees(Connection* conn);
void getEmployee(struct Employee *emp);
void insertEmployee(Connection* conn, struct Employee emp);
void updateEmployee(Connection* conn, int employeeNumber);
void deleteEmployee(Connection *conn, int employeeNumber);


int main()
{
	Environment* env = nullptr;
	Connection* conn = nullptr;
	
	string user = "dbs211_212c11";
	string pass = "14315115";
	string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";

	int option, found, employeeNum;
	Employee emp;

	try
	{
		//Create the environment and connect to database
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);

		//Menu interface for user selection
		do {
			option = menu();

			switch (option)
			{
			case 0:
				break;

			case 1:
				cout << "Enter Employee Number: ";
				cin >> employeeNum;
				
				found = findEmployee(conn, employeeNum, &emp);

				if (!found)
				{
					cout << "Employee " << employeeNum << " does not exist." << endl << endl;
				}
				else
				{
					displayEmployee(conn, emp);
				}
				break;


			case 2:
				displayAllEmployees(conn);
				break;

			case 3:
				getEmployee(&emp);
				insertEmployee(conn, emp);
				break;

			case 4:
				cout << "Employee Number: ";
				cin >> employeeNum;
				updateEmployee(conn, employeeNum);
				break;

			case 5:
				cout << "Employee Number: ";
				cin >> employeeNum;
				deleteEmployee(conn, employeeNum);
				break;

			}


		} while (option != 0);

		//Terminate database connection and terminate the environment
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& sqlExcp)
	{
		//Display error code and error message for sql exception
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();

	}

	return 0;

}



int menu(void)
{
	int option;
	bool valid = true;

	cout << "\n********************* HR Menu *********************" << endl;
	cout << "1) Find Employee" << endl;
	cout << "2) Employees Report" << endl;
	cout << "3) Add Employee" << endl;
	cout << "4) Update Employee" << endl;
	cout << "5) Remove Employee" << endl;
	cout << "0) Exit" << endl;
	cout << "Enter an option(0 - 5): ";

	do
	{
		cin >> option;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid option. Please try again: ";
			valid = false;
		}
		else if (option < MIN_OPTION || option > MAX_OPTION)
		{
			cout << "Option must be between " << MIN_OPTION << " and " << MAX_OPTION << ": ";
			valid = false;
		}
		else
		{
			valid = true;
		}

	} while (!valid);

	return option;

}


int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp)
{
	int found = 0;
	Statement* stmt = nullptr;

	stmt = conn->createStatement("SELECT * FROM dbs211_employees WHERE employeenumber = :1");
	stmt->setInt(1, employeeNumber);

	ResultSet* rs = stmt->executeQuery();

	if (rs->next())
	{
		found = 1;
		emp->employeeNumber = rs->getInt(1);
		strcpy_s(emp->lastName, (rs->getString(2)).c_str());
		strcpy_s(emp->firstName, (rs->getString(3)).c_str());
		strcpy_s(emp->extension, (rs->getString(4)).c_str());
		strcpy_s(emp->email, (rs->getString(5)).c_str());
		strcpy_s(emp->officecode, (rs->getString(6)).c_str());
		emp->reportsTo = rs->getInt(7);
		strcpy_s(emp->jobTitle, (rs->getString(8)).c_str());
	}

	conn->terminateStatement(stmt);

	return found;
}


void displayEmployee(Connection* conn, struct Employee emp)
{
	cout << "\n-------------- Employee Information -------------" << endl;
	cout << "Employee Number: " << emp.employeeNumber << endl;
	cout << "Last Name: " << emp.lastName << endl;
	cout << "First Name: " << emp.firstName << endl;
	cout << "Extension: " << emp.extension << endl;
	cout << "Email: " << emp.email << endl;
	cout << "Office Code: " << emp.officecode << endl;
	cout << "Manager ID: " << emp.reportsTo << endl;
	cout << "Job Title: " << emp.jobTitle << endl;

}


void displayAllEmployees(Connection *conn)
{
	Statement* stmt = nullptr;

	stmt = conn->createStatement("SELECT a.employeenumber, a.firstname || ' ' || a.lastname, a.email, c.phone, a.extension, b.Firstname || ' ' || b.lastname FROM dbs211_employees a LEFT OUTER JOIN dbs211_employees b ON a.reportsto = b.employeenumber INNER JOIN dbs211_offices c ON a.officecode = c.officecode ORDER BY a.employeenumber");

	ResultSet* rs = stmt->executeQuery();

	if (!rs->next())
	{
		cout << "There is no employees’ information to be displayed." << endl;
	}
	else
	{
		//Formating for report header
		cout << "\n-----   ---------------   ------------------------------   -----------------   ---------   -----------------" << endl;
		cout << "ID      Employee Name     Email                            Phone               Extension   Manager Name     " << endl;
		cout << "-----   ---------------   ------------------------------   -----------------   ---------   -----------------" << endl;


		//Print the result set row by row
		do
		{
			cout.setf(ios::left);
			cout.width(8);
			cout << rs->getInt(1);
			cout.width(18);
			cout << rs->getString(2);
			cout.width(33);
			cout << rs->getString(3);
			cout.width(20);
			cout << rs->getString(4);
			cout.width(12);
			cout << rs->getString(5);
			cout.width(17);
			cout << rs->getString(6) << endl;
			cout.unsetf(ios::left);

		} while (rs->next());

	}

	conn->terminateStatement(stmt);

}



void getEmployee(struct Employee *emp)
{
	cout << "-------------- New Employee Information -------------" << endl;

	cout << "Employee Number: ";
	cin >> emp->employeeNumber;
	cin.get();

	cout << "Last Name: ";
	cin.getline(emp->lastName, 50);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(1000, '\n');
	}


	cout << "First Name: ";
	cin.getline(emp->firstName, 50);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(1000, '\n');
	}

	cout << "Extension: ";
	cin.getline(emp->extension, 10);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(1000, '\n');
	}

	cout << "Email: ";
	cin.getline(emp->email, 99);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(1000, '\n');
	}

	cout << "Office Code: 1" << endl;
	strcpy_s(emp->officecode, "1");
	cout << "Manager ID: 1002" << endl;
	emp->reportsTo = 1002;

	cout << "Job Title: ";
	cin.getline(emp->jobTitle, 50);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(1000, '\n');
	}

}


void insertEmployee(Connection* conn, struct Employee emp)
{
	int found;
	Statement* stmt = nullptr;

	found = findEmployee(conn, emp.employeeNumber, &emp);

	if (found)
	{
		cout << "\nAn employee with the same employee number exists." << endl;
	}
	else
	{
		stmt = conn->createStatement("INSERT INTO dbs211_employees VALUES (:1, :2, :3, :4, :5, :6, :7, :8)");
		stmt->setInt(1, emp.employeeNumber);
		stmt->setString(2, emp.lastName);
		stmt->setString(3, emp.firstName);
		stmt->setString(4, emp.extension);
		stmt->setString(5, emp.email);
		stmt->setString(6, emp.officecode);
		stmt->setInt(7, emp.reportsTo);
		stmt->setString(8, emp.jobTitle);
		stmt->executeUpdate();

		conn->commit();
		conn->terminateStatement(stmt);

		cout << "\nThe new employee is added successfully." << endl;
	}

}



void updateEmployee(Connection *conn, int employeeNumber)
{
	int found;
	Statement* stmt = nullptr;
	Employee emp = {};

	found = findEmployee(conn, employeeNumber, &emp);

	if (found)
	{
		cout << "Last Name: " << emp.lastName << endl;
		cout << "First Name: " << emp.firstName << endl;
		cout << "Extension: ";
		cin >> emp.extension;

		stmt = conn->createStatement("UPDATE dbs211_employees SET extension = :1 WHERE employeenumber = :2");
		stmt->setString(1, emp.extension);
		stmt->setInt(2, emp.employeeNumber);
		stmt->executeUpdate();
		conn->commit();
		conn->terminateStatement(stmt);

		cout << "The employee's extension is updated successfully." << endl;
	}
	else
	{
		cout << "The employee with ID " << employeeNumber << " does not exist." << endl;
	}

}



void deleteEmployee(Connection *conn, int employeeNumber)
{
	int found;
	Statement* stmt = nullptr;
	Employee emp = {};

	found = findEmployee(conn, employeeNumber, &emp);

	if (found)
	{
		stmt = conn->createStatement("DELETE FROM dbs211_employees WHERE employeenumber = :1");
		stmt->setInt(1, employeeNumber);
		stmt->executeUpdate();
		conn->commit();
		conn->terminateStatement(stmt);

		cout << "The employee with ID " << employeeNumber << " is deleted successfully." << endl;
	}
	else
	{
		cout << "The employee with ID 1000 does not exist." << endl;
	}

}
