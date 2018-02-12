#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <string>
#include <iomanip>
#include <cassert>
#include <map>
#include <vector>
#include <bitset>
#include <sstream>

typedef unsigned long ulong;
using namespace std;

class bool_int{

public:

bool_int(){}

unsigned long int fromVector(vector<bool> vec){
unsigned long int val = 0;
int i =0;
for (vector<bool>::iterator it = vec.begin(); it!=vec.end(); it++){

       val = val+ ( vec[i] * pow(2,i));
       i++;

}
return val;
}

};

struct configuration{

int m;
int k;

};



int main(int argc, char *argv[]){

  configuration config_obj;
  ifstream inFile;
  inFile.open(argv[1]);
  bool_int bool_int_object;

  // initializing miss counters and number of traces
  int misspredictions=0;
  int total_traces=0;
  int predicted_branch = 2;




  while(!inFile.eof())  // read config file
  {

    inFile>>config_obj.m>>config_obj.k;

    }


    int size_m, size_k;

    size_m = (int) pow(2, (double) config_obj.m);
    size_k = (int) pow(2, (double) config_obj.k) ;

    //initializing predictor_table with STRONGLY_TAKEN

    bitset<2> predictor_table[size_m][size_k];
    for(int i = 0; i<size_m - 1; i++){
      for(int j =0; j<size_k - 1; j++){
        predictor_table[i][j] = 11;
      }
    }

    //initializing Branch history register with TAKEN

    bitset<2> bhr[size_k];
    for(int i=0;i<size_k - 1; i++){
      bhr[i] = 1;
  }
  // converting bitset to strings
  string bhr_str[size_k];
  for(int j=0;j<size_k - 1; j++){
     bhr_str[j] = bhr[j].to_string();
  }
    //concatenate the strings ;
    for(int j=0;j<size_k - 1; j++){
      bhr_str[j] = bhr_str[j] + bhr_str[j+1];
    }


    //converting it into bitset
    std::bitset<32> bhrr(bhr_str[0]);


    //converting bitset integer
    int bhr_int = bhrr.to_ulong();

  //input and output trace file

    ifstream trace_file;
    ofstream traceout_file;
    string output_file;
    output_file = string(argv[2])+".out";
    trace_file.open(argv[2]);
    traceout_file.open(output_file.c_str());


    char orig_predictor;

    unsigned int addr;
    bitset<32> accessaddr;


//
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string hexaddr;

    if(trace_file.is_open() && traceout_file.is_open()){
      while(getline(trace_file,line)){

        // reading the trace file
        istringstream iss(line);
        if (!(iss >> hexaddr)) {break;}
        stringstream saddr(hexaddr);
        saddr >> std::hex >> addr;
        accessaddr = bitset<32> (addr);     // read the 32 bit PC address to accessaddr

         orig_predictor= line[9] - '0';           // read the original branch to orig_predictor

        int orig_int = int(orig_predictor);       // converting the original branch to int

        vector<bool> tempbits(config_obj.m) ;

        // step to transfer the last m bits of PC access address to tempbits
        for(int c=0;c<=config_obj.m-1;c++){

          tempbits[c] = accessaddr[c] ;

        }

       // converting temp bits(last m bits) to integer value for access to table

         int mbits_int = bool_int_object.fromVector(tempbits);

         //int bhr_int = stoi(bhr_str);

         if((predictor_table[mbits_int][bhr_int]) == 11){


              predicted_branch = 1;


              if(predicted_branch == orig_int){


              }
              else{

                misspredictions++;

                predictor_table[mbits_int][bhr_int] = 10;


              }

              int temp = bhr[size_k-1].to_ulong();
              for(int i = size_k-1; i>=1; i--){
                bhr[i] = bhr[i-1];
              }


              bhr[0] = orig_predictor;


         }
         else if(predictor_table[mbits_int][bhr_int] == 10){


                 predicted_branch = 1;

              if(predicted_branch == orig_int){

               predictor_table[mbits_int][bhr_int] =11;

              }
              else{

                misspredictions++;

               predictor_table[mbits_int][bhr_int] = 00;


              }
             int temp = bhr[size_k-1].to_ulong();
              for(int i = size_k-1; i>=1; i--){
                bhr[i] = bhr[i-1];
              }


              bhr[0] = orig_predictor;
         }

         else if(predictor_table[mbits_int][bhr_int] == 01){


                 predicted_branch = 0;

              if(predicted_branch == orig_int){

               predictor_table[mbits_int][bhr_int] =00;

              }
              else{

               misspredictions++;

               predictor_table[mbits_int][bhr_int] =11;


              }

              int temp = bhr[size_k-1].to_ulong();
              for(int i = size_k-1; i>=1; i--){
                bhr[i] = bhr[i-1];
              }


              bhr[0] = orig_predictor;
         }

         else{


                 predicted_branch = 0;


              if(predicted_branch == orig_int){


              }
              else{

               misspredictions++;

               predictor_table[mbits_int][bhr_int]=01;


              }
              int temp = bhr[size_k-1].to_ulong();
              for(int i = size_k-1; i>=1; i--){
                bhr[i] = bhr[i-1];
              }


              bhr[0] = orig_predictor;

         }




        traceout_file<<" "<< predicted_branch << endl;  // Output hit/miss results for L1 and L2 to the output file;

         total_traces++;



    }
    trace_file.close();
    traceout_file.close();

    double percentage=(misspredictions*100.0/total_traces);
}

else cout<< "Unable to open trace file! ";


return 0;
}
