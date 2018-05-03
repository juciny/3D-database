#include <iostream>
#include <string>
#include <mysql.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")
using namespace std;

class conndba
{
private:
	MYSQL mydata;
public:
	conndba()
	{
		//��ʼ�����ݿ�
		if (0 == mysql_library_init(0, NULL, NULL))
		{
			cout << "mysql_library_init() succeed" << endl;
		}
		else { cout << "mysql_library_init() failed" << endl; }
		//��ʼ�����ݽṹ
		if (NULL != mysql_init(&mydata)) {
			cout << "mysql_init() succeed" << endl;
		}
		else {
			cout << "mysql_init() failed" << endl;
		}

		//���������ݿ�֮ǰ�����ö��������ѡ��
		//�������õ�ѡ��ܶ࣬���������ַ����������޷���������
		if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {
			cout << "mysql_options() succeed" << endl;
		}
		else {
			cout << "mysql_options() failed" << endl;
		}
		//�������ݿ�
		if (NULL
			!= mysql_real_connect(&mydata, "localhost", "root", "", "db001", 3306, NULL, 0))
			//����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������
		{
			cout << "mysql_real_connect() succeed" << endl;
		}
		else {
			cout << "mysql_real_connect() failed" << endl;
		}
	}

	string select_file(string name)
	{
		string file_name;
		string sqlstr;
		sqlstr = "SELECT model FROM models WHERE name='" + name + "'";
		MYSQL_RES *result = NULL;
		if (0 == mysql_query(&mydata, sqlstr.c_str())) {
			cout << "mysql_query() select data succeed" << endl;
			//һ����ȡ�����ݼ�
			result = mysql_store_result(&mydata);
			//ȡ�ò���ӡ����
			int rowcount = mysql_num_rows(result);    //��ȡ����
			cout << "row count: " << rowcount << endl;

			if (rowcount == 0)
			{
				file_name = "NULL";
				cout << file_name << endl;
			}
			else {
				//ȡ�ò���ӡ���ֶε�����
				unsigned int fieldcount = mysql_num_fields(result);
				MYSQL_ROW row = NULL;
				row = mysql_fetch_row(result);
				file_name = row[0];
				cout << file_name << endl;
			}
			return file_name;

		}
		else {
			cout << "mysql_query() select data failed" << endl;
			mysql_close(&mydata);
			return NULL;
		}

		mysql_free_result(result);
		mysql_close(&mydata);
		mysql_server_end();
	}
	
};

//int main() {
//	conndba *demo = new conndba();
//
//	string file_name;
//	file_name = demo->select_file("chair");
//
//	cout << "file name is" << file_name << endl;
//
//	return 0;
//}
