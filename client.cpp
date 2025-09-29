/*
	Original author of the starter code
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 2/8/20
	
	Please include your Name, UIN, and the date below
	Name:
	UIN:
	Date:
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <chrono>

using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	bool newchannel = false;
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:c")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				break;
		        case 'c':
			  newchannel = true;
			  break;
		}
	}
	pid_t pid = fork();
	if(pid == 0){
	  char* server_args[] = {(char*)"./server",nullptr};
	  execvp("./server",server_args);
	  
	}
    FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);

	if(newchannel == true)
		  {
		    MESSAGE_TYPE msg = NEWCHANNEL_MSG;
		    chan.cwrite(&msg, sizeof(MESSAGE_TYPE));
		    char newChannel[50];
		    chan.cread(newChannel, sizeof(newChannel));
			
		FIFORequestChannel nc(newChannel, FIFORequestChannel::CLIENT_SIDE);

		if(e==1 && t ==0.0)
	  {
	    ofstream outfile("received/x1.csv");
	    for(int i = 0; i<1000; i++)
	      {
		datamsg x1(p,i*0.004,1);
		char buf[MAX_MESSAGE];

		memcpy(buf,&x1, sizeof(datamsg));
		nc.cwrite(buf,sizeof(datamsg));
		double ecg1;
		nc.cread(&ecg1,sizeof(double));

		
		datamsg x2(p,i*0.004,2);
		memcpy(buf,&x2,sizeof(datamsg));
		nc.cwrite(buf,sizeof(datamsg));
		double ecg2;
		nc.cread(&ecg2,sizeof(double));

		outfile << i*0.004 << "," << ecg1 << "," << ecg2 <<endl;
		
	      }
	    outfile.close();
	  }
	if(filename!=""){
	  auto start = chrono::high_resolution_clock::now();
	  int length;
	  filemsg gl(0,0);
	  length = sizeof(gl) + filename.size() + 1;
	  char* buf = new char[length];
	  memcpy(buf,&gl,sizeof(filemsg));
	  strcpy(buf+sizeof(filemsg), filename.c_str());
	  nc.cwrite(buf, length);

	  __int64_t fileSize;
	  nc.cread(&fileSize, sizeof(__int64_t));
	  
	  // delete buf[];

	  __int64_t offset = 0;
	  string newfilename = "received/"+filename;
	  FILE* outfile = fopen(newfilename.c_str(),"wb");
	  while (offset<fileSize)
	    {
	      int chunk = 0;
	      if(MAX_MESSAGE>fileSize-offset){
		chunk = fileSize-offset;}
	      else{chunk = MAX_MESSAGE;}
	      
	      filemsg dataReq(offset,chunk);
	      memcpy(buf, &dataReq, sizeof(filemsg));
	      strcpy(buf + sizeof(filemsg), filename.c_str());
	      nc.cwrite(buf,length);

	      char* data = new char[chunk];
	      nc.cread(data,chunk);

	      fwrite(data,1,chunk,outfile);
	      
	      offset+=chunk;
	      delete[]data;
	    }
	  fclose(outfile);
	  delete[] buf;
	  auto end = chrono::high_resolution_clock::now();
	  auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);

	  cout << fileSize << " " << "bytes " << "in " << duration.count() << " milliseconds" <<endl;
	}
	    	char buf[MAX_MESSAGE]; // 256
    		datamsg x(p, t, e);
	
		memcpy(buf, &x, sizeof(datamsg));
		nc.cwrite(buf, sizeof(datamsg)); // question
		double reply;
		nc.cread(&reply, sizeof(double)); //answer
		cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;

		  }
	else{

	// example data point request
	if(e==1 && t ==0.0)
	  {
	    ofstream outfile("received/x1.csv");
	    for(int i = 0; i<1000; i++)
	      {
		datamsg x1(p,i*0.004,1);
		char buf[MAX_MESSAGE];

		memcpy(buf,&x1, sizeof(datamsg));
		chan.cwrite(buf,sizeof(datamsg));
		double ecg1;
		chan.cread(&ecg1,sizeof(double));

		
		datamsg x2(p,i*0.004,2);
		memcpy(buf,&x2,sizeof(datamsg));
		chan.cwrite(buf,sizeof(datamsg));
		double ecg2;
		chan.cread(&ecg2,sizeof(double));

		outfile << i*0.004 << "," << ecg1 << "," << ecg2 <<endl;
		
	      }
	    outfile.close();
	  }
	if(filename!=""){
	  auto start = chrono::high_resolution_clock::now();
	  int length;
	  filemsg gl(0,0);
	  length = sizeof(gl) + filename.size() + 1;
	  char* buf = new char[length];
	  memcpy(buf,&gl,sizeof(filemsg));
	  strcpy(buf+sizeof(filemsg), filename.c_str());
	  chan.cwrite(buf, length);

	  __int64_t fileSize;
	  chan.cread(&fileSize, sizeof(__int64_t));
	  
	  // delete buf[];

	  __int64_t offset = 0;
	  string newfilename = "received/"+filename;
	  FILE* outfile = fopen(newfilename.c_str(),"wb");
	  while (offset<fileSize)
	    {
	      int chunk = 0;
	      if(MAX_MESSAGE>fileSize-offset)
		{
		  chunk = fileSize-offset;
		}
	      else{
		chunk = MAX_MESSAGE;
	      }
	      
	      filemsg dataReq(offset,chunk);
	      memcpy(buf, &dataReq, sizeof(filemsg));
	      strcpy(buf + sizeof(filemsg), filename.c_str());
	      chan.cwrite(buf,length);

	      char* data = new char[chunk];
	      chan.cread(data,chunk);

	      fwrite(data,1,chunk,outfile);
	      
	      offset+=chunk;
	      delete[]data;
	    }
	  fclose(outfile);
	  delete[] buf;
	  auto end = chrono::high_resolution_clock::now();
	  auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);

	  cout << fileSize << " " << "bytes " << "in " << duration.count() << " milliseconds" <<endl;

	}
	    	char buf[MAX_MESSAGE]; // 256
    		datamsg x(p, t, e);
	
		memcpy(buf, &x, sizeof(datamsg));
		chan.cwrite(buf, sizeof(datamsg)); // question
		double reply;
		chan.cread(&reply, sizeof(double)); //answer
		cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
	}
		
    // sending a non-sense message, you need to change this
	
	/*	filemsg fm(0, 0);
	string fname = "teslkansdlkjflasjdf.dat";
	
	int len = sizeof(filemsg) + (fname.size() + 1);
	char* buf2 = new char[len];
	memcpy(buf2, &fm, sizeof(filemsg));
	strcpy(buf2 + sizeof(filemsg), fname.c_str());
	chan.cwrite(buf2, len);  // I want the file length;
	
	delete[] buf2;
	*/
	
	// closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite(&m, sizeof(MESSAGE_TYPE));
}
