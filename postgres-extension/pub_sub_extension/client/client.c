#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <pthread.h>
#include <string.h>

void do_exit(PGconn *conn) {    
    PQfinish(conn);
    exit(1);
}

PGconn *conn;

void connect_stream(char *client_id){
	char query[100];
	conn = PQconnectdb("host=seil hostaddr=10.129.149.32 password=thenine@seil user=seil port=5432 dbname=postgres");
	if (PQstatus(conn) == CONNECTION_BAD) {
		PQerrorMessage(conn);
		printf("unsuccessful connect\n");
		//do_exit(conn);
	}
	sprintf(query,"select connect_stream('%s');",client_id);
	printf("%s\n",query);
	PGresult *res = PQexec(conn, query);
	printf("%d\n\n",PQresultStatus(res));
	if (PQresultStatus(res)!=PGRES_TUPLES_OK) {
        printf("stream conncection unsuccessful\n");        
        PQclear(res);
        do_exit(conn);
    }
    printf("stream conncection successful\n");
}

int publish(char *client_id, char *topic, char *payload){
	char query[100];
	sprintf(query,"select publish('%s', '%s', '%s')",client_id, topic, payload);
	PGresult *res = PQexec(conn, query);
	if(PQresultStatus(res)!=PGRES_COMMAND_OK){
		printf("publish didn't worked for %s.",client_id);
		PQclear(res);
	}
}

struct thread_args{
	char *client_id;
	char *topic;
	int timeout;
	void (*callbackfn)();
};


void mysubscribe(void *args){
	char query[100];
	int rows,i;


	struct thread_args *args1 = (struct thread_args *)args; 
	sprintf(query, "select subscribe('%s', '%s', '%d')",args1->client_id, args1->topic, args1->timeout);
	while(1){
		PGresult *res = PQexec(conn, query);
		if(PQresultStatus(res)!=-1) break;     /*  check if this nomenclature may work on how to send data from server side. */ 

		rows = PQntuples(res);
		for(i = 0; i<rows; i++)
			(*args1->callbackfn)(PQgetvalue(res, i ,0 /* topic */), PQgetvalue(res, i ,1 /* payload timestamp*/), PQgetvalue(res, i ,2/* payload */));
		PQclear(res);
	}		
}

int subscribe(char *client_id, char *topic, int timeout, void (*callbackfn)()){
	/*pthread_t thread_id;
	struct thread_args *args = malloc(sizeof(struct thread_args));
	strcpy(args->client_id, client_id);
	args->timeout = timeout;
	args->callbackfn = callbackfn;
	pthread_create(&thread_id, NULL, (void*)mysubscribe, (void*)args);	
	pthread_detach(thread_id);*/
	char query[1000];
	int rows,i;


	sprintf(query, "select subscribe('%s', '%s', '%d')",client_id, topic, timeout);
	while(1){
		PGresult *res = PQexec(conn, query);
		//if(PQresultStatus(res)!=-1) break;     /*  check if this nomenclature may work on how to send data from server side. */ 

		rows = PQntuples(res);
		for(i = 0; i<rows; i++)
			(*callbackfn)(PQgetvalue(res, i ,0 /* topic */), PQgetvalue(res, i ,1 /* payload timestamp*/), PQgetvalue(res, i ,2/* payload */));
		PQclear(res);
	}
}

void callback(char* topic, char* payload_timestamp, char* payload){
	printf("%s %s %s\n", topic, payload_timestamp, payload);
}

int main(){
	connect_stream("client5");
	//publish("client2", "topic1", "first message");
	subscribe("client5", "topic1", 1000, callback);
	PQfinish(conn);
	//while(1);

}
