commitid 206427f33510215f5457b5c830c43bb16c0295e2

My rather quickly implemented `memmove` function doesn't always does what it should.

### Notes

-   ```none
    % p
    Hello, world!
    Another line!
    % a
    yay.
    .
    % p
    Hello, world!
    Another line!
    yay.
    % 0a
    x
    .
    % p
    x
    Hlo, world!
    Another line!
    yay.
    ```

    Technically, this could be something else, however, I strongly suspect that my `memmove` implementation
    is pretty broken.

### Ideas

### Actions

-   This was a typo when I was working on `memmove`.
    I always wrote to the same address.
