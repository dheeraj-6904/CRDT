#include<bits/stdc++.h>
using namespace std;

int main(){
    string filedata = "file.txt-line1 still line 1\nline2 akf \n finally line 3";
    std::istringstream iss(filedata);
    std::string filename, content;

    // Extract the filename and the content separated by '-'
    std::getline(iss, filename, '-');  //Extract up to '-'
    //std::getline(iss, content);         // Extract the rest as content
    std::cout<<filedata<<"\n";
    std::cout<<filename<<" ------  "<< content<<"\n";
    string word;
    while(getline(iss,word)){
        word = word+"\n";
        content.append(word);
    }
    //std::getline(iss, content, '\0');
    cout<<content;
}