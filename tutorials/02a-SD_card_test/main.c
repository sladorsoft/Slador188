#include <stdio.h>
#include <pico/stdlib.h>
#include <sd_card.h>


static spi_t SPIDev =
{
    .hw_inst = spi0,
    .miso_gpio = PICO_DEFAULT_SPI_RX_PIN,
    .mosi_gpio = PICO_DEFAULT_SPI_TX_PIN,
    .sck_gpio = PICO_DEFAULT_SPI_SCK_PIN,
    .baud_rate = 10 * 1000 * 1000
};

static sd_spi_if_t SPIInterface =
{
    .spi = &SPIDev,
    .ss_gpio = PICO_DEFAULT_SPI_CSN_PIN
};

static sd_card_t SDCards[] =
{
    {
        .type = SD_IF_SPI,
        .spi_if_p = &SPIInterface,
        .use_card_detect = false
    }
};


size_t sd_get_num() { return count_of(SDCards); }

sd_card_t* sd_get_by_num(size_t idx)
{
    if (idx >= sd_get_num())
    {
        return NULL;
    }

    return &SDCards[idx];
}


int main()
{
    stdio_init_all();

    sd_init_driver();

    sd_card_t* card = sd_get_by_num(0);
    int result = card->init(card);
    printf("\nInit result: %i.\n", result);
    printf("The card has %u sectors.\n\n", card->get_num_sectors(card));

    uint8_t buffer[512];
    card->read_blocks(card, buffer, 0, 1);

    printf("Sector 0:\n");
    for (int i = 0; i < 512; i++)
    {
        printf("%2.2x ", buffer[i]);
        if ((i % 16) == 15)
        {
            printf("\n");
        }
    }

    while (true)
    {
        __wfi();
    }
}
