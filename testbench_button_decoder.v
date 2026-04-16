`timescale 1ns / 1ps

module testbench_button_decoder();

reg clk;
reg [3:0] data;
reg strobe;
reg reset;
wire [11:0] buttons;

button_decoder uut(
    .clk(clk),
    .data(data),
    .strobe(strobe),
    .reset(reset),
    .buttons(buttons)
);

// Генерация тактового сигнала 50MHz
initial begin
    clk = 0;
    forever #10 clk = ~clk;
end

// Задача для сброса
task do_reset;
    begin
        reset = 1;
        repeat(5) @(posedge clk);
        reset = 0;
        repeat(5) @(posedge clk);
    end
endtask

// Задача для отправки данных
task send;
    input [3:0] val;
    begin
        @(posedge clk);
        data = val;
        @(posedge clk);
        strobe = 1;
        @(posedge clk);
        strobe = 0;
        @(posedge clk);
    end
endtask

// Задача для проверки
task check;
    input [11:0] exp;
    input [255:0] name;
    begin
        @(negedge clk);
        #5;
        if(buttons === exp)
            $display("OK [%s] buttons = %b", name, buttons);
        else
            $display("ERROR [%s] Expected %b, Got %b", name, exp, buttons);
    end
endtask

initial begin
  $display("Button Decoder Teset");
    
    data = 0;
    strobe = 0;
    reset = 1;
    #100;
    reset = 0;
    #100;
    
    // Тест 1: Одна кнопка Key0
    do_reset();
    $display("\n[Teset 1] Key0 only");
    send(4'b0001);
    check(12'b000000000001, "Test1");
    #1000;
    
    // Тест 2: Кнопки Key0 и Key1
    do_reset();
    $display("\n[Test 2] Key0 and Key1");
    send(4'b0001);
    send(4'b0010);
    check(12'b000000000011, "Test2");
    #1000;
    
    // Тест 3: Кнопки Key0, Key1, Key2
    do_reset();
    $display("\n[Test 3] Key0, Key1, Key2");
    send(4'b0001);
    send(4'b0010);
    send(4'b0011);
    check(12'b000000000111, "Test3");
    #1000;
    
    // Тест 4: Все 12 кнопок
    do_reset();
    $display("\n[Test 4] All 12 buttons");
    send(4'b0001);
    send(4'b0010);
    send(4'b0011);
    send(4'b0100);
    send(4'b0101);
    send(4'b0110);
    send(4'b0111);
    send(4'b1000);
    send(4'b1001);
    send(4'b1010);
    send(4'b1011);
    send(4'b1100);
    check(12'b111111111111, "Test4");
    #1000;
    
    // Тест 5: Комбинация (Key0, Key3, Key6, Key9)
    do_reset();
    $display("\n[Test 5] Key0, Key3, Key6, Key9");
    send(4'b0001);
    send(4'b0000);
    send(4'b0000);
    send(4'b0100);
    send(4'b0000);
    send(4'b0000);
    send(4'b0111);
    send(4'b0000);
    send(4'b0000);
    send(4'b1010);
    send(4'b0000);
    send(4'b0000);
    check(12'b001001001001, "Test5");
    #1000;
    
    // Тест 6: Автосброс по таймауту
    do_reset();
    $display("\n[Test 6] Auto-reset after timeout");
    send(4'b0001);
    send(4'b0010);
    check(12'b000000000011, "Test6a");
    
    $display("Waiting 50ms");
    #50000000;
    check(12'b000000000000, "Test6b");
    #1000;
    
    // Тест 7: Проверка работы сброса
    do_reset();
    $display("\n[Test 7] Hardware reset test");
    send(4'b0001);
    send(4'b0010);
    send(4'b0011);
    check(12'b000000000111, "Test7a");
    
    $display("Applying reset");
    reset = 1;
    @(posedge clk);
    @(posedge clk);
    reset = 0;
    check(12'b000000000000, "Test7b");
    #1000;
    
    $display("Stop Test");
    
    #10000;
    $finish;
end

endmodule
