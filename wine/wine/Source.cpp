#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*range que sera calculado
(0,105) valores de treinamento
(105,142) valores de teste1
(142,178) valores de teste2*/
int max, min;

const double aprendizado = 0.0000025;
double lamb = 13.5;

float x[14][178];
double teta[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};

void leDados()
{
	FILE *p;
	p = fopen("wine.data","r");

	float d = 0.0;
	char c;

	for(int i = 0; i < 178; i++)
		for(int j = 0; j < 14; j++)
		{
			fscanf_s(p, "%f", &d);
			x[j][i] = d;
			
			//le a virgula que separa os valores
			c = getc(p);
		}

	fclose(p);
}

void defineRange(int inicio, int fim)
{
	max = fim;
	min = inicio;
}

double hx(int i)
{
	//bias
	double resultado = teta[0];

	//teta*x para cada teta
	for(int j = 1; j < 14; j++)
		resultado += teta[j] * x[j][i];

	return resultado;
}

void gradiente()
{
	double temp[14] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double erro = 0.0;

	//para o bias
	for(int i = min; i < max; i++)
			erro += (hx(i)-x[0][i]);
		
	temp[0] = teta[0] - aprendizado * 1/(max - min) * erro;
	
	//para cada teta
	for(int j = 1; j < 14; j++)
	{
		erro = 0.0;
		for(int i = min; i < max; i++)
			erro += (hx(i)-x[0][i])*x[j][i];
		
		temp[j] = teta[j] - aprendizado * ((1.0/(max - min))*erro + (lamb/(max - min))*teta[j]);
	}

	//atribui os valores aos tetas
	for(int j = 0; j < 14; j++)
		teta[j] = temp[j];
}

double regularizacao()
{
	double temp = 0.0;
	
	for(int j = 0; j < 14; j++)
		temp += pow(teta[j],2);

	return (lamb * temp);//coeficiente de regularizacao
}

double jx()
{
	double erro = 0;
	//somatoria do quadrado dos erros
	for(int i = min; i <max; i++)
   		erro += pow((hx(i)-x[0][i]),2);
	
	return ((1.0/(2*(max - min))) * (erro + regularizacao()));
}

void guardaTeta(double ultimo_erro, int count)
{
	FILE *p;
	p = fopen ("teta.txt", "w");

	//teta
	for(int i = 0; i < 14; i++)
		fprintf (p, "%f;",teta[i]);

	//erro e count
	fprintf (p,"%f; erro %.6f, count %d",lamb, ultimo_erro, count);

	fclose(p);
}

int confereErro(double *ultimo_erro, int count)
{
	double erro = 0;

    erro = jx();

	if(count%100000 == 0)
	{
		printf("J(x) = %f \tDeltaJ(x) = %.12f\tCount = %d\n", erro, (*ultimo_erro - erro), count);
		//guardaTeta(erro, count);
	}

	//confere se o erro ainda esta diminuindo
	if((*ultimo_erro - erro) < 0.0000000000005 && (*ultimo_erro - erro) > 0.0)
		return 2;

	*ultimo_erro = erro;

	if(erro > 0.02)
		return 1; //erro inaceitavel
	else
		return 0;//erro aceitavel
}

void calculaTeta()
{
	defineRange(0,105); //trabalha apenas com ~60% dos casos de teste
	int count = 0;
	double ultimo_erro = 1.0, primeiro_erro = 0.0;

	printf("Modelo:\nteta0 + teta1*x1 + teta2*x2 + ... + teta13*x13\n\nResultados esperados para h(x):\n1, 2 ou 3\n\n");

	//pega valor do erro inicial
	gradiente();
	count++;
	confereErro(&ultimo_erro, count);
	primeiro_erro = ultimo_erro;
	printf("Erro inicial = %f\n", primeiro_erro);
	gradiente();
	count++;

	printf("Calculando, por favor aguarde!\n\n");

	//loop para achar os tetas
	while(confereErro(&ultimo_erro, count) == 1)
	{
		gradiente();
		count++;
	}

	//adiciona valores de teta a teta.txt
	guardaTeta(ultimo_erro, count);

	//caso erro = 0.03 nao seja possivel, imprime erro inicial e menor erro encontrado
	if(confereErro(&ultimo_erro, count) == 2)
		printf("Erro inicial = %f\tErro minimo encontrado = %f\n",primeiro_erro, ultimo_erro);

	//valores finais de teta e numero de passos
	printf("teta0 = %f\tteta1 = %f\tteta2 = %f\tteta3 = %f\nteta4 = %f\tteta5 = %f\tteta6 = %f\nteta7 = %f\tteta8 = %f\tteta9 = %f\nteta10 = %f\tteta11 = %f\tteta12 = %f\nteta13 = %f\n", teta[0], teta[1], teta[2], teta[3], teta[4], teta[5], teta[6], teta[7], teta[8], teta[9], teta[10], teta[11], teta[12], teta[13]);
	printf("numero de passos: %d\n", count);

	system("pause");
}

void leValores()
{
	FILE *p;
	p = fopen ("teta.txt", "r");
	char c;
	float f;

	//tetas
	for(int j = 0; j < 14; j++)
	{
		fscanf (p, "%f",&f);
		teta[j] = f;
		c = getc(p);//pega virgula
	}

	//lamb
	fscanf(p, "%f", &f);
	lamb = f;

	fclose(p);
	//printf("teta 0 = %f\tteta1 = %f\tteta2 = %f\tteta3 = %f\nteta4 = %f\tteta5 = %f\tteta6 = %f\nteta7 = %f\tteta8 = %f\tteta9 = %f\nteta10 = %f\tteta11 = %f\tteta12 = %f\nteta13 = %f\n", teta[0], teta[1], teta[2], teta[3], teta[4], teta[5], teta[6], teta[7], teta[8], teta[9], teta[10], teta[11], teta[12], teta[13]);

}

void guardaTeste()
{
	FILE *p;
	p = fopen ("teste.txt", "w+");

	//vai ate o final do arquivo - nao esta funcionando!
	char c = getc(p);
	while(c != EOF)
		c = getc(p);
	
	fprintf(p, "\nlambida = %f\nTeta:\n", lamb);
	for(int i = 0; i < 14; i++)
		fprintf (p, "%f;",teta[i]);

	defineRange(0,105); //valores de treinamento
	fprintf(p, "\nJ(x) de valores de treinamento = %f\n", jx());
	//printf("Erro de valores de treinamento = %f\n", jx());

	defineRange(105,142); //valores de teste1
	fprintf(p, "J(x) de valores de teste 1 = %f\n", jx());
	//printf("Erro de valores de teste 1 = %f\n", jx());

	defineRange(142,178); //valores de teste2
	fprintf(p,"J(x) de valores de teste 2 = %f\n\n", jx());
	//printf("Erro de valores de teste 2 = %f\n\n", jx());

	//casos em que o erro gera decisao errada
	fprintf(p, "Valores com erro inaceitavel:\n");
	int errado, inaceitavel;
	errado = inaceitavel = 0;

	for(int i = 0; i < 178; i++)
	{
		float erro = hx(i)-x[0][i];
		if(erro > 0.5 || erro < -0.5)
		{
			errado ++;
			if(erro > 1.0 || erro < -1.0)
				inaceitavel ++;
			printf("i = %d\tesperado = %f\tobtido = %f\terro = %f\n",i, x[0][i], hx(i), erro);

		}
	}
	fprintf(p, "Erros acima de 0.5 = %d\tErros acima de 1.0 = %d\n", errado-inaceitavel, inaceitavel);

	fclose(p);
}

void testaTeta()
{
	leValores();
	guardaTeste();
	printf("Valores guardados com sucesso!\nVerifique o arquivo teste.txt\n");
	system("pause");
}

void main()
{
	leDados();

	while(true)
	{
		int aux = 0;
		printf("Digite:\n1 para calcular teta\n2 para testar teta\n0 para sair\n");
		scanf("%d",&aux);
		if(aux == 1)
			calculaTeta();
		if(aux == 2)
			testaTeta();
		else
			exit(0);
	}
}