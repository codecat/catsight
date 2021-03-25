#include <Common.h>
#include <Random.h>

static s2::list<s2::string> g_animals = {
	"Aardvark", "Albatross", "Alligator", "Alpaca", "Anole", "Ant", "Anteater", "Antelope", "Ape", "Armadillo", "Donkey", "Baboon", "Badger", "Barracuda", "Bat", "Bear", "Beaver", "Bee", "Binturong", "Bird", "Bison", "Bluebird", "Boar", "Bobcat", "Budgerigar", "Butterfly", "Camel", "Capybara", "Caracal", "Caribou", "Cassowary",
	"Cat", "Caterpillar", "Cattle", "Chamois", "Cheetah", "Chicken", "Chimpanzee", "Chinchilla", "Chough", "Coati", "Cobra", "Cockroach", "Cod", "Cormorant", "Cougar", "Coyote", "Crab", "Crocodile", "Crow", "Cuckoo", "Curlew", "Deer", "Dhole", "Dingo", "Dinosaur", "Dog",
	"Squalidae", "Dolphin", "Donkey", "Dove", "Dragonfly", "Duck", "Dugong", "Dunlin", "Eagle", "Echidna", "Eel", "Taurotragus", "Elephant", "Elk", "Emu", "Falcon", "Ferret", "Finch", "Fish", "Flamingo", "Fly", "Fox", "Frog", "Gaur", "Gazelle", "Gecko", "Gerbil", "Giraffe",
	"Gnat", "Wildebeest", "Goat", "Goose", "Gorilla", "Goshawk", "Grasshopper", "Grouse", "Guanaco", "Gull", "Hamster", "Hare", "Hawk", "Hedgehog", "Heron", "Herring", "Hippopotamus", "Hoatzin", "Hoopoe", "Hornet", "Horse", "Human", "Hummingbird", "Hyena", "Ibex", "Ibis", "Iguana", "Impala", "Jackal", "Jaguar", "Jay", "Jellyfish",
	"Jerboa", "Kangaroo", "Kingbird", "Kingfisher", "Kinkajou", "Koala", "Kodkod", "Kookaburra", "Kouprey", "Kudu", "Lapwing", "Lark", "Lechwe", "Lemur", "Leopard", "Lion", "Lizard", "Llama", "Lobster", "Locust", "Loris", "Louse", "Lynx", "Lyrebird", "Macaque", "Macaw", "Magpie", "Mallard", "Mammoth", "Manatee", "Mandrill", "Margay",
	"Marmoset", "Marmot", "Meerkat", "Mink", "Mongoose", "Monkey", "Moose", "Mosquito", "Mouse", "Myna", "Narwhal", "Newt", "Nightingale", "Nilgai", "Ocelot", "Octopus", "Okapi", "Oncilla", "Opossum", "Orangutan", "Oryx", "Ostrich", "Otter", "Ox", "Owl", "Oyster", "Parrot", "Panda", "Partridge", "Peafowl", "Penguin", "Pheasant", "Pigeon",
	"Pika", "Pony", "Porcupine", "Porpoise", "Pug", "Quail", "Quelea", "Quetzal", "Rabbit", "Raccoon", "Sheep", "Rat", "Raven", "Reindeer", "Rhinoceros", "Salamander", "Salmon", "Sandpiper", "Sardine", "Seahorse", "Pinniped", "Serval", "Shark", "Sheep", "Shrew", "Shrike", "Siamang", "Skink",
	"Skunk", "Sloth", "Snail", "Snake", "Spider", "Spoonbill", "Squid", "Squirrel", "Starling", "Stilt", "Swan", "Tamarin", "Tapir", "Tarsier", "Termite", "Tiger", "Toad", "Topi", "Toucan", "Turaco", "Turtle", "Vicuña", "Thelyphonida", "Viper", "Vulture", "Wallaby", "Walrus", "Wasp", "Waxwing", "Weasel", "Whale", "Wobbegong",
	"Wolf", "Wolverine", "Wombat", "Woodpecker", "Worm", "Wren", "Zebra"
};

s2::string Random::Animal()
{
	return g_animals[rand() % g_animals.len()];
}
