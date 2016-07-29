<?php
 
//You can of course choose any name for your class or integrate it in something like a functions or base class
class LoginFormKey
{
    //Here we store the generated form key
    private $loginFormKey;
     
    //Here we store the old form key (more info at step 4)
    private $old_loginFormKey;
     
    //The constructor stores the form key (if one excists) in our class variable
    function __construct()
    {
        //We need the previous key so we store it
        if(isset($_SESSION['_login_formKey']))
        {
            $this->old_loginFormKey = $_SESSION['_login_formKey'];
        }
    }
 
    //Function to generate the form key
    private function generateKey()
    {
        //Get the IP-address of the user
        $ip = $_SERVER['REMOTE_ADDR'];
         
        //We use mt_rand() instead of rand() because it is better for generating random numbers.
        //We use 'true' to get a longer string.
        //See http://www.php.net/mt_rand for a precise description of the function and more examples.
        $uniqid = uniqid(mt_rand(), true);
         
        //Return the hash
        return md5($ip . $uniqid);
    }
 
     
    //Function to output the form key
    public function outputKey()
    {
        //Generate the key and store it inside the class
        $this->loginFormKey = $this->generateKey();
        //Store the form key in the session
        $_SESSION['_login_formKey'] = $this->loginFormKey;
         
        //Output the form key
        echo "<input type='hidden' name='_login_formKey' id='_login_formKey' value='".$this->loginFormKey."' />";
    }
 
     
    //Function that validated the form key POST data
    public function validate()
    {
        //We use the old formKey and not the new generated version
        if($_POST['_login_formKey'] == $this->old_loginFormKey)
        {
            //The key is valid, return true.
            return true;
        }
        else
        {
            //The key is invalid, return false.
            return false;
        }
    }
}
?>