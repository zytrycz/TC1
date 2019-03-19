#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream> 
#include <cmath>
#include <sstream>
#include <jsoncpp/json/json.h>

using namespace cv;
using namespace std;
using namespace Json; 

/*Global variables*/
Value jsonStruct;
String jsonExm;
int promedioRImg;
int promedioGImg;
int promedioBImg;
int promedioLImg; //Promedio de luma
int maxAlpha = 0;
int alphaX;
int alphaY;
float sat = 0;
int satX;
int satY;

void luz(){
	cout<<endl;
	cout<<"la zona mas iluminada esta en el rango:  x\t, y"<<endl;
	int iniX;
	int iniY;
	int finX;
	int finY;
	//Sacar los valores de x
	if ( ((alphaX-84)+200) >1024){
		finX=1024;
		iniX=824;
	}
	else if (alphaX-84<0){
		finX=200;
		iniX=0;
	}
	else if(alphaX==0){
		iniX=satX;
		finX=200;
	}
	else{
		iniX=alphaX-84;
		finX=iniX+200;
	}
	//Sacar los valores de y
	if (((alphaY-84)+200)>1024){
		finY=1024;
		iniY=824;
	}
	else if (alphaY-84<0){
		finY=200;
		iniY=0;
	}
	else if(alphaY==0){
		iniY=satY;
		finY=200;
	}
	else{
		iniY=alphaY-84;
		finY=iniY+200;
	}
	cout<<"                                       (";
	cout<<iniX<<"\t,"<<iniY<<")"<<endl;
	cout<<"                                       (";
	cout<<finX<<"\t,"<<finY<<")"<<endl;
}

void saturacion(){
	cout<<endl;
	cout<<"la zona mas saturada esta en el rango:  x\t, y"<<endl;
	int iniX;
	int iniY;
	int finX;
	int finY;
	//Sacar los valores de x
	if ( ((satX-84)+200) >1024){
		finX=1024;
		iniX=824;
	}
	else if (satX-84<0){
		finX=200;
		iniX=0;
	}
	else if(satX==0){
		iniX=satX;
		finX=200;
	}
	else{
		iniX=satX-84;
		finX=iniX+200;
	}
	//Sacar los valores de y
	if (((satY-84)+200)>1024){
		finY=1024;
		iniY=824;
	}
	else if (satY-84<0){
		finY=200;
		iniY=0;
	}
	else if(satY==0){
		iniY=satY;
		finY=200;
	}
	else{
		iniY=satY-84;
		finY=iniY+200;
	}
	cout<<"                                       (";
	cout<<iniX<<"\t,"<<iniY<<")"<<endl;
	cout<<"                                       (";
	cout<<finX<<"\t,"<<finY<<")"<<endl;
}

void mainColor(){
	cout<<"El color predominante es el ";
	if (promedioRImg<=77 && promedioGImg<=77 && promedioBImg<=77) 
		cout<<"Negro"<<endl;
	else if (promedioLImg<50 && ((promedioGImg>=92 && promedioBImg>=76) ||
								(promedioGImg>=92 && promedioRImg>=66) ||
								(promedioRImg>=66 && promedioBImg>=76) ||
								(promedioGImg>=92 && promedioRImg>=66 && promedioBImg>=76)))
		cout<< "Blanco"<<endl;
	else if (promedioLImg>=50 && fmax(fmax(promedioRImg,promedioGImg),promedioBImg)==promedioRImg ) 
		cout<< "Rojo"<<endl;
	else if (promedioLImg>=50 && fmax(fmax(promedioRImg,promedioGImg),promedioBImg)==promedioGImg ) 
		cout<< "Verde"<<endl;
	else if (promedioLImg>=50 && fmax(fmax(promedioRImg,promedioGImg),promedioBImg)==promedioBImg ) 
		cout<< "Azul"<<endl;
	else{
			int hueR = promedioRImg/255;
			int hueG = promedioGImg/255;
			int hueB = promedioBImg/255;
			if ( promedioRImg> promedioBImg && promedioRImg> promedioGImg)
				if (50<= ( 
					 (promedioGImg-promedioBImg)/(fmax(fmax(promedioRImg,promedioGImg),promedioBImg)-fmin(fmin(promedioRImg,promedioGImg),promedioBImg))
				    )<= 70) cout<< "Amarillo"<<endl;
			if ( promedioGImg> promedioBImg && promedioGImg> promedioRImg)
				if (50<= 2+( 
					 (promedioBImg-promedioRImg)/(fmax(fmax(promedioRImg,promedioGImg),promedioBImg)-fmin(fmin(promedioRImg,promedioGImg),promedioBImg))
				    )<= 70) cout<< "Amarillo"<<endl;
			if ( promedioBImg> promedioGImg && promedioBImg> promedioRImg)
				if (50<= 4+( 
					 (promedioRImg-promedioGImg)/(fmax(fmax(promedioRImg,promedioGImg),promedioBImg)-fmin(fmin(promedioRImg,promedioGImg),promedioBImg))
				    )<= 70) cout<< "Amarillo"<<endl;		
		}
	
}

void crearJSON(int x, int y,int pR, int pG,int pB,float luma, float alpha){
	jsonStruct["X"] = x;
	jsonStruct["Y"] = y;
	jsonStruct["pR"] = pR;
	promedioRImg += pR;
	jsonStruct["pG"] = pG;
	promedioGImg += pG;
	jsonStruct["pB"] = pB;
	promedioBImg += pB;
	jsonStruct["luma"] = luma;
	promedioLImg += luma;
	jsonStruct["alpha"] = alpha;
	StyledWriter styledWriter;
	jsonExm += styledWriter.write(jsonStruct);
	
	
}

void getValues(int x, int y, Mat chunk){
	int pB = 0;
	int pG = 0;
	int pR = 0;
	for(int i = 0; i < chunk.rows; i++) //Recorre pixel por pixel obteniendo el valor BGR
	{
		for(int j = 0; j < chunk.cols; j++)
		{
			pB += chunk.at<Vec3b>(i,j)[0];
			pG += chunk.at<Vec3b>(i,j)[1];
			pR += chunk.at<Vec3b>(i,j)[2];
		}
	}
	//Sacar promedios
	pB /= 1024; //son 1024 pixeles por chunk
	pG /= 1024;
	pR /= 1024;
    float luma = 1-(fmin(fmin(pR,pG),pB)/fmax(fmax(pR,pG),pB));
    luma *= 100;
    
    float alpha = sqrt((pR*pR*0.241)+(0.691*pG*pG)+(0.068*pB*pB));
    
    if (alpha> maxAlpha){ //de esta manera conforme revisa va dejando el chunk mas luminoso marcado
		maxAlpha=alpha;
		alphaX = x;
		alphaY = y; 
	}
	if (luma> sat){ //de esta manera conforme revisa va dejando el chunk mas luminoso marcado
		sat=luma;
		satX = x;
		satY = y; 
	}
    
    crearJSON(x,y,pR,pG,pB,luma,alpha);
}

void progra()
{
	string name;
	cout<<"-Ingrese la ruta de la imagen a analizar\n-Debe de ser de 1024x1024\n-Si la imagen se encuentra en la misma carpeta que este programa\nsolo ingrese el nombre\n-No ingrese la extencion al final del nombre de la imagen por favor\n"<<endl;
	cin>>name;
	//def vars
	jsonStruct["X"] = 0;
	jsonStruct["Y"] = 0;
	jsonStruct["pR"] = 0;
	jsonStruct["pG"] = 0;
	jsonStruct["pB"] = 0;
	jsonStruct["luma"] = 0;
	jsonStruct["alpha"] = 0;
	/*debe meterse en un ciclo para cada imagen*/
	Mat image;
    image = imread(name+".jpg");
    int x = 0;
    int y = 0;
    Mat subMat;
	int cont = 1;
	
	while (y!=1024){
		/*Se encarga de "separar" la imagen en chuncks
		 * subMat es igual a un chunk de 32x32 pxl
		 * nos movemos al siguiente chunk de 32x32 sobre el eje x
		 * dado que las imagenes son de 1024x1024 cuando x=1024
		 * significa que leyo toda esa fila, entonces se devuelve a la primer columna y baja una fila
		 * cuando y=1024 es que ya leyo todas las filas, por ende termino
		 * */
		stringstream ss;
		ss<<cont;
		jsonExm += ss.str()+"\n";
		cont++;		
		subMat = image(Rect(x,y,32,32)); 
		getValues(x,y,subMat); //obtener los valores necesarios
		x+=32; 
		if (x==1024){ 
			x = 0;
			y+= 32;
		}	
	}
	
	promedioRImg /= 1024; //son 1024 chunks
	promedioGImg /= 1024;
	promedioBImg /= 1024;
	promedioLImg /= 1024;
	
	mainColor();
	luz();
	saturacion();
	ofstream myfile (name+".json");
    if (myfile.is_open())
    {
    	myfile << jsonExm;
	    myfile.close();
    }
}

int main(){
	int opc=1;
	while(opc){
		if(opc) progra();
		cout<<"Ingrese 1 si desea analizar otra imagen\n O ingrese 0 para salir"<<endl;
		cin>>opc;
	}
	
	return 0;
}
