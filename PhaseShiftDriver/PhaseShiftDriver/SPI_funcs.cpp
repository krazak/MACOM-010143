#include "stdafx.h"
#include "SPI_funcs.h"




void SPI_configuration(unsigned char configuration)
{
	P1 |= 0xF0; /* programming SPI pins high */
	SPCR = configuration;
}
