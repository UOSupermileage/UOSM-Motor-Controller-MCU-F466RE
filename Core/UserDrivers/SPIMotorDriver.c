/*
 * SPIDriver.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#include <SPIMotorDriver.h>

#include "SerialDebugDriver.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

extern SPI_HandleTypeDef hspi1;

const uint32_t timeout = 5;


// => SPI wrapper
/**
 * Return the result of writing a single byte.
 */
uint8_t tmc4671_readwriteByte(const uint8_t motor, uint8_t data, uint8_t lastTransfer)
{
	HAL_StatusTypeDef status;

	// Pointer to receive buffer. (Can be interpreted as an array with a signle byte.
	uint8_t *rx_data;

	// Size == 1 because we will receive a signle uint8_t AKA a single byte
	status = HAP_SPI_TransmitReceive(&hspi1, &data, rx_data, 1, timeout);

	return *rx_data;
}
// <= SPI wrapper

void initMotor() {
	// Motor type &  PWM configuration
	tmc4671_writeInt(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, 0x00030004);
	tmc4671_writeInt(0, TMC4671_PWM_POLARITIES, 0x00000000);
	tmc4671_writeInt(0, TMC4671_PWM_MAXCNT, 0x00000F9F);
	tmc4671_writeInt(0, TMC4671_PWM_BBM_H_BBM_L, 0x00001919);
	tmc4671_writeInt(0, TMC4671_PWM_SV_CHOP, 0x00000007);

	// ADC configuration
	tmc4671_writeInt(0, TMC4671_ADC_I_SELECT, 0x24000100);
	tmc4671_writeInt(0, TMC4671_dsADC_MCFG_B_MCFG_A, 0x00100010);
	tmc4671_writeInt(0, TMC4671_dsADC_MCLK_A, 0x20000000);
	tmc4671_writeInt(0, TMC4671_dsADC_MCLK_B, 0x00000000);
	tmc4671_writeInt(0, TMC4671_dsADC_MDEC_B_MDEC_A, 0x014E014E);
	tmc4671_writeInt(0, TMC4671_ADC_I0_SCALE_OFFSET, 0xFF007FE3);
	tmc4671_writeInt(0, TMC4671_ADC_I1_SCALE_OFFSET, 0xFF0080E9);

	// Digital hall settings
	tmc4671_writeInt(0, TMC4671_HALL_MODE, 0x00000001);
	tmc4671_writeInt(0, TMC4671_HALL_PHI_E_PHI_M_OFFSET, 0x00000000);

	// Feedback selection
	tmc4671_writeInt(0, TMC4671_PHI_E_SELECTION, 0x00000005);
	tmc4671_writeInt(0, TMC4671_VELOCITY_SELECTION, 0x0000000C);

	// Limits
	tmc4671_writeInt(0, TMC4671_PID_TORQUE_FLUX_LIMITS, 0x000003E8);

	// PI settings
	tmc4671_writeInt(0, TMC4671_PID_TORQUE_P_TORQUE_I, 0x01000100);
	tmc4671_writeInt(0, TMC4671_PID_FLUX_P_FLUX_I, 0x01000100);

	// ===== Digital hall test drive =====

	// Switch to torque mode
	tmc4671_writeInt(0, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000001);


	uint32_t nPolePairs = tmc4671_readInt(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS);

	if (nPolePairs == 0x00030004) {
		DebugPrint("nPolePairs successfully read/write to tmc4671");
	} else {
		DebugPrint("Failed to read/write nPolePairs to tmc4671");
	}

	DebugPrint("Finished motor init");
}

void rotateMotorRight() {
	tmc4671_writeInt(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0x03E80000);

}

void rotateMotorLeft() {
	tmc4671_writeInt(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0xFC180000);

}

void stopMotor() {
	tmc4671_writeInt(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0x00000000);

}