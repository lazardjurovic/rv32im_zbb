
int niz[10] = {28,3,11,45,55,2,7,4,100,21};

int main(){
	int i,j,tmp;

    for(i = 0; i<10;i++){
        for(int j = 0; j<10; j++){
            if(niz[i] < niz[j]){
                tmp = niz[i];
                niz[i] = niz[j];
                niz[j] = tmp;
            }
        }
    }
	
	int a = 5;
	int b = 10;
	int c = a+b;
	return 0;
}