import java.io.*;

class Naloga2 {

	public static void main(String[] args) throws IOException{

		String file = read_file(args[0]);
		save_file(args[1], fix(file));

	}

	//main function
	//fix the order of paragraphs and call fix_words and fix_sentences on each paragraph
	public static String fix(String str){
		String[] paragraphs = str.split("\n");
		int par_num = paragraphs.length;

		//reorder paragraphs
		for(int i = 0; i < par_num / 2; i+=2){
			String buffer = paragraphs[i]; //save the string

			int target_par = par_num - 1 - i - 1 + par_num % 2; //calculate the index of target paragraph

			//switch them
			paragraphs[i] = paragraphs[target_par];
			paragraphs[target_par] = buffer;
		}

		//join array back into a string and call fix_words and fix_sentences for each paragraph
		String ret = "";
		for(int i = 0; i < par_num; i++){
			ret += fix_sentences(fix_words(paragraphs[i]));

			if(i != par_num - 1){
				ret += "\n";
			}
		}
		return ret;
	}

	//fix the order of words
	public static String fix_words(String str){
		String[] words = str.split(" ");
		int words_len = words.length;
		int i = words_len - 1;

		//go from the end of array towards the start
		while(i >= 1){

			if(words[i].contains(".")){
				//if the sentence has an odd number of words the last word stays where it is
				i += -1;
			}else{
				//if the sentence has an even number of words all other words are switched
				do{
					//switch words
					String buffer = words[i];
					words[i] = words[i - 1];
					words[i - 1] = buffer;

					i += -2;

					if(i < 1){
						break;
					}
				}while(!words[i].contains(".") && !words[i - 1].contains("."));
			}
		}

		String ret = "";
		for(i = 0; i < words_len; i++){
			ret += flip_string(words[i]);

			if(i != words_len - 1){
				ret += " ";
			}
		}

		return ret;

	}


	//fix the order of sentences
	public static String fix_sentences(String str){
		String[] sentences = (str + " ").split("\\. ");
		int sentences_len = sentences.length;

		String ret = "";
		//reverse order
		for(int i = 0; i < sentences_len; i++){
			if(i != sentences_len - 1){
				//reattach ". "
				ret += sentences[sentences_len - 1 - i] + ". ";
			}else{
				//remove the trailing whitespace
				ret += sentences[0] + ".";
			}
		}

		return ret;
	}

	//flip a word
	public static String flip_string(String str){
		String new_string = new String();

		int strlen = str.length();
		//reverse string character by character
		for(int i = 0; i < strlen; i++){
			new_string += str.charAt(strlen - 1 - i); //add chars back to front
		}

		return new_string;
	}


	//read from a file
	public static String read_file(String path) throws IOException {

		StringBuilder builder = new StringBuilder();

		FileReader freader = new FileReader(path);
		BufferedReader reader = new BufferedReader(freader);

		String line;
		while ((line = reader.readLine()) != null) {
			builder.append(line);
			builder.append("\n");
		}
		//delete trailing \n
		builder.deleteCharAt(builder.length() - 1);

		reader.close();

		return builder.toString();
	}

	//save to the file
	public static void save_file(String path, String str) throws IOException {

		File file = new File(path);

		if (!file.exists()) {
			file.createNewFile();
		}

		FileWriter fwriter = new FileWriter(path);
	    BufferedWriter writer = new BufferedWriter(fwriter);

		writer.write(str);
	    writer.close();
	}
}
