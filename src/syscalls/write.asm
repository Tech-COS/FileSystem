section .note.GNU-stack noalloc noexec nowrite progbits
section .text
global write_byte

wait_for_ready:
    mov dx, 0x1F7
.wait:
    in al, dx           ; Read the status register into AL.
    test al, 0x80       ; Check the busy bit (bit 7) to see if the drive is busy.
    jnz .wait           ; If the busy bit is set (non-zero), jump back to .wait and loop until the drive is ready.
    test al, 0x40
    jz .wait
    ret                 ; Return from the function once the drive is ready.

wait_for_ready_next:
    mov dx, 0x1F7
    in al, dx
    in al, dx
    in al, dx
    in al, dx
.wait:
    in al, dx           ; Read the status register into AL.
    test al, 0x80       ; Check the busy bit (bit 7) to see if the drive is busy.
    jnz .wait           ; If the busy bit is set (non-zero), jump back to .wait and loop until the drive is ready.
    test al, 0x8
    jz .wait

write_loop:
    mov dx, 0x1F0
    mov rcx, 256
.loop:
    mov ax, [rdi]          ; Stocke le mot dans le buffer
    out dx, ax             ; Écrit un mot (16 bits) depuis le disque
    jmp .delay
.delay:
    add rdi, 2             ; Incrémente le pointeur de buffer
    loop .loop

    dec rbx                ; Move to the next sector
    test rbx, rbx
    jne wait_for_ready_next
    ret

; Every write command must end with a Cache Flush (0xE7) command.
complete_write_command:
    mov dx, 0x1F7
    mov ax, 0xE7
    out dx, ax
    ret

; 48bit LBAs == 6 bytes.
; 16bit sectorcount == 2 bytes.
; A sectorcount of 0 == uint16 MAX == 32MB
; Each LBA number corresponds to the beginning of a 512 byte block.
; A 512 byte block is called a sector.
write_byte:
    pushfq
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax

    xor rax, rax
    mov rbx, rdx
    ; Select Master Drive
    mov dx, 0x1F6
    mov al, 0x40
    out dx, al

    ; Send the sectorcount high bits to port 0x1F2
    mov dx, 0x1F2
    mov al, bh
    out dx, al

    ; Send LBA byte 4 to 0x1F3:
    ror rsi, 24
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 5 to 0x1F4:
    ror rsi, 8
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 6 to 0x1F5:
    ror rsi, 8
    inc dx
    mov al, sil
    out dx, al

    rol rsi, 40

    ; Send the sectorcount low bits to port 0x1F2
    mov dx, 0x1F2
    mov al, bl
    out dx, al

    ; Send LBA byte 1 to 0x1F3
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 2 to 0x1F4
    shr rsi, 8
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 3 to 0x1F5
    shr rsi, 8
    inc dx
    mov al, sil
    out dx, al

    ; Send the "WRITE SECTORS EXT" command (0x24) to port 0x1F7
    mov dx, 0x1F7
    mov al, 0x34
    out dx, al

    ; Wait until the disc is ready
    call wait_for_ready

    ; Write Data
    call write_loop

    call complete_write_command
    call wait_for_ready

    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    popfq
    ret
