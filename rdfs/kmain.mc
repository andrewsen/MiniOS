import "kernel.sem"
//import "kmath.sem"

public void main()
{
	double n1, n2;
	print("Enter 2 numbers:\n");
	n1 = readd();
	n2 = readd();
	
	print("Enter action:\n");
	string act = reads();
	switch(act)
	{
		case "+":
			println(n1+n2);
			break;
		case "-":
			println(n1-n2);
			break;
		case "*":
			println(n1*n2);
			break;
		case "/":
			println(n1/n2);
			break;
		default:
			println("unknown action");
			break;
	}
	
}
