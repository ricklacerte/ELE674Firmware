/*
 * Motor.c
 *
 *  Created on: 12 sept. 2013
 *      Author: bruno
 */


#include "Motor.h"

#define TimeDelay 50000

extern sem_t	MotorTimerSem;
extern int		MotorActivated;

pthread_barrier_t 	MotorStartBarrier;


int gpio_set (int nr, int val)  {
	char cmdline[200];

	if (val < 0)
		sprintf(cmdline, "/usr/sbin/gpio %d -d i", nr);
	else if (val > 0)
		sprintf(cmdline, "/usr/sbin/gpio %d -d ho 1", nr);
	else
		sprintf(cmdline, "/usr/sbin/gpio %d -d ho 0", nr);

	return system(cmdline);
}


int motor_open(void) {
	struct termios config;
	int uart = open(MOTOR_UART, O_RDWR | O_NOCTTY | O_NDELAY);

	if (uart < 0) {
		printf("motor_open : impossible d'ouvrir le uart du moteur\n");
		return uart;
	}

	fcntl(uart, F_SETFL, 0); //read calls are non blocking

	//set port config
	tcgetattr(uart, &config);
	cfsetspeed(&config, B115200);
	config.c_cflag |= (CLOCAL | CREAD); //Enable the receiver and set local mode
	config.c_iflag = 0; //clear input config
	config.c_lflag = 0; //clear local config
	config.c_oflag &= ~OPOST; //clear output config (raw output)
	cfmakeraw(&config);
	tcsetattr(uart, TCSANOW, &config);
	return uart;
}

int motor_cmd(int file, uint8_t cmd, uint8_t *reply, int replylen) {
	int size;

	write(file, &cmd, 1);
	fsync(file);
	usleep(TimeDelay);
	size = read(file, reply, replylen);

	return size;
}

// initialiser le moteurs (devrait etre appelé dans la phase d'initialisation)
int MotorPortInit(MotorStruct *Motor) {
	uint8_t reply[256];
	int		i;

	//open motor port
	Motor->file = motor_open();
	if (Motor->file < 0) {
		printf("motor_open: Impossible d'ouvrir le UART\n");
		return Motor->file;
	}
	//reset IRQ flipflop - this code resets GPIO_ERROR_READ to 0
	gpio_set(GPIO_ERROR_RESET, 0);
	usleep(2*TimeDelay);

	//all select lines inactive
	gpio_set(GPIO_M1, 0);
	gpio_set(GPIO_M2, 0);
	gpio_set(GPIO_M3, 0);
	gpio_set(GPIO_M4, 0);
	usleep(2*TimeDelay);

	//configure motors
	for (i = 0; i < 4; ++i) {
		gpio_set(GPIO_M1 + i, -1);
		usleep(2*TimeDelay);
		motor_cmd(Motor->file, 0xE0, reply, 2);
		motor_cmd(Motor->file, 0x91, reply, 121);
		motor_cmd(Motor->file, 0xA1, reply, 2);
		motor_cmd(Motor->file, i + 1, reply, 1);
		motor_cmd(Motor->file, 0x40, reply, 2);
		gpio_set(GPIO_M1 + i ,0);
		usleep(2*TimeDelay);
	}

	//all select lines active
	gpio_set(GPIO_M1, -1);
	gpio_set(GPIO_M2, -1);
	gpio_set(GPIO_M3, -1);
	gpio_set(GPIO_M4, -1);
	usleep(2*TimeDelay);

	gpio_set(GPIO_ERROR_READ, -1);
	usleep(2*TimeDelay);

	return 0;
}


void motor_send(MotorStruct *Motor, int SendMode) {
/* Fonction utilitaire pour simplifier les transmissions aux moteurs envoyer une trame au moteur */
// implémenter un tableau, avec le code approprié, et le transmettre (série)  vers les moteurs
	switch (SendMode) {
	case MOTOR_NONE : 		break;
	case MOTOR_PWM_ONLY :	/* A faire! */
							break;
	case MOTOR_LED_ONLY :	/* A faire! */
							break;
	case MOTOR_PWM_LED :	/* A faire! */
							break;
	}
}


// MOTOR TASK 
void *MotorTask ( void *ptr ) {
/* A faire! */
/* Tache qui transmet les nouvelles valeurs de vitesse */
/* à chaque moteur à interval régulier (5 ms).         */
	while (MotorActivated) {
//		DOSOMETHING();
	}
	pthread_exit(0); /* exit thread */
}

//MOTOR INIT
int MotorInit (MotorStruct *Motor) {
/* A faire! */
/* Ici, vous devriez faire l'initialisation des moteurs.   */
/* C'est-à-dire initialiser le Port des moteurs avec la    */
/* fonction MotorPortInit() et créer la Tâche MotorTask()  */
/* qui va s'occuper des mises à jours des moteurs en cours */ 
/* d'exécution.                                            */
	return 0;
}


//MOTOR START
int MotorStart (void) {
/* A faire! */
/* Ici, vous devriez démarrer la mise à jour des moteurs (MotorTask).    */ 
/* Tout le système devrait être prêt à faire leur travail et il ne reste */
/* plus qu'à tout démarrer.                                              */
	return retval;
}


//MOTOR STOP
int MotorStop (MotorStruct *Motor) {
/* A faire! (motor activated à 0)*/
/* Ici, vous devriez arrêter les moteurs et fermer le Port des moteurs. */ 
	return 0;
}

