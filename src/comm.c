
// ... (earlier code remains unchanged)

    // Fixed snprintf usage
    snprintf(buf, sizeof(buf), "\x02\x06%s tells the group '%s'.\x02\x01\n\r", ch->name, argument);

    // ... (other code in between)

// In function ‘do_delete’
    // Fixed system() call
    (void)system(buf);

// ... (rest of the code remains unchanged)
