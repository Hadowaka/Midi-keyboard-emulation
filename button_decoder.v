`timescale 1ns / 1ps

module button_decoder(
    input clk,              // 50MHz тактовый сигнал
    input [3:0] data,       // 4 бита данных с Arduino
    input strobe,           // Строб-импульс
    input reset,            // сброс
    output reg [11:0] buttons  // 12 кнопок на выходе
);

reg [3:0] slot = 0;           // Текущий слот
reg [11:0] buffer = 0;        // Буфер для накопления
reg strobe_prev = 0;          // Предыдущее состояние строба
reg [23:0] timeout_cnt = 0;   // Счетчик таймаута

localparam TIMEOUT_MAX = 24'd2_000_000;  // 40ms при 50MHz

always @(posedge clk) begin
    strobe_prev <= strobe;
    
    if(reset) begin
        slot <= 0;
        buffer <= 0;
        buttons <= 0;
        timeout_cnt <= 0;
        strobe_prev <= 0;
    end
    else if(strobe && !strobe_prev) begin
        timeout_cnt <= 0;
        
        // Обновляем буфер
        buffer[slot] <= (data != 0);
        
        // Обновляем выход из буфера
        buttons <= buffer;
        
        // Следующий слот
        if(slot == 11)
            slot <= 0;
        else
            slot <= slot + 1;
    end
    else begin
        if(timeout_cnt < TIMEOUT_MAX)
            timeout_cnt <= timeout_cnt + 1;
        else begin
            slot <= 0;
            buffer <= 0;
            buttons <= 0;
            timeout_cnt <= 0;
        end
    end
end

endmodule
