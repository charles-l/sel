# sel
simple cli menu selection

Run `vim` on selected file:

`ls | sel vim`

Bind each specified editor to the first letter of its name (with vim as the default if return is used):

`ls | sel -V vim -E ed -N nano vim`
