#include "woody.h"

void	*open_file(char **argv, unsigned int *fsize)
{
	int			fd;
	void		*f;
	off_t		start;
	off_t		size;

	if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
		ft_printf("woody_woodpacker: '%s': Couldn't open file\n", argv[1]);
		return (NULL);
	}
	start = lseek(fd, (size_t)0, SEEK_CUR);
	size = lseek(fd, (size_t)0, SEEK_END);//get file size
	lseek(fd, start, SEEK_SET); //set cursor back at the beg
	f = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	*fsize = size;
	return (f == MAP_FAILED ? NULL : f);
}

int parse_magic(t_elf_file ef)
{
	if (sizeof(Elf32_Ehdr) > ef.fsize)
	{
		ft_printf("woody_woodpacker: %s: file format not recognized\n", ef.fname);
		return (0);
	}
	ft_memcpy(&ef.elf32header, ef.file, sizeof(Elf32_Ehdr));
	if (ef.elf32header.e_ident[EI_MAG0] != ELFMAG0		||
		ef.elf32header.e_ident[EI_MAG1] != ELFMAG1		||
		ef.elf32header.e_ident[EI_MAG2] != ELFMAG2  	||
		ef.elf32header.e_ident[EI_MAG3] != ELFMAG3		||
	 	ef.elf32header.e_ident[EI_CLASS] == ELFCLASSNONE)
	{
		ft_printf("woody_woodpacker: %s: file format not recognized\n", ef.fname);
		return (0);
	}
	if (ef.elf32header.e_ident[EI_CLASS] == ELFCLASS32)
	{
		if (parse32elf(ef))
			ft_printf("woody_woodpacker: File corrupted\n");
	}
	else if (ef.elf32header.e_ident[EI_CLASS] == ELFCLASS64)
	{
		ft_memcpy(&ef.elf64header, ef.file, sizeof(Elf64_Ehdr));
		if (ef.elf64header.e_type != ET_EXEC && ef.elf64header.e_type != ET_DYN)
			printf("woody_woodpacker: %s is not an executable\n", ef.fname);
		else if (parse64elf(ef))
			ft_printf("woody_woodpacker: File corrupted\n");
	}
	return (0);
}

int open_wfile()
{
	int fd;

	if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, S_IRWXO | S_IRWXU | S_IRWXG)) < 0)
	{
		printf("woody_woodpacker: could not create new binary\n");
	}
	return (fd);
}

char *generate_key()
{
	char *ret;
	int fd;
	if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
		return (NULL);
	if (!(ret = ft_calloc(1, DEFAULT_KEY_LEN + 1)))
		return (NULL);	
	if (read(fd, ret, DEFAULT_KEY_LEN) < 0)
	{
		free(ret);
		return (NULL);
	}
	for (int i = 0; i < DEFAULT_KEY_LEN - 1; i++)
		if (!ret[i])
			ret[i] = 's';
	ret[DEFAULT_KEY_LEN] = 0;
	return (ret);
} 

int ft_woody(void *file, char *fname, unsigned int fsize, char *key)
{
	t_elf_file	ef;
	ef.fname = fname;
	ef.file = file;
	ef.fsize = fsize;
	
	if (!key)
		ef.key = generate_key();
	else
		ef.key = ft_strdup(key);
	if (!ef.key)
	{
		printf("woody_woodpacker: could not generate key\n");
		return (1);
	}
	printf("key_value: ");
	for (size_t i = 0; i < ft_strlen(ef.key); i++)
		printf("\\x%hhx", ef.key[i]);
	printf("\n");
	if ((ef.wfd = open_wfile()) < 0)
	{
		free(ef.key);
		return (1);
	}
	if (parse_magic(ef))
	{
		free(ef.key);
		return (1);
	}
	free(ef.key);
	return (0);
}


int		main(int argc, char **argv)
{
	void			*file;
	unsigned int	fsize;

	char **arg_cpy;

	if (!(arg_cpy = copy_sp(argv)))
		return (1);
	if (argc < 2)
	{
		free_sp(arg_cpy);
		if (!(arg_cpy = malloc(sizeof(char *) * 3)))
			return (1);
		if (!(arg_cpy[0] = ft_strdup(argv[0])))
		{
			free(arg_cpy);
			return (1);
		}
		if (!(arg_cpy[1] = ft_strdup("a.out")))
		{
			free(arg_cpy);
			free(arg_cpy[0]);
			return (1);
		}
		arg_cpy[2] = NULL;
		argc = 2;
	}
	if (argc == 2 || argc == 3)
	{
		if (ft_strnstr(arg_cpy[1], "woody", ft_strlen(arg_cpy[1])) )
			printf("woody_woodpacker: You devil hacker you used the forbiden word gtfo of here\n");
		else if ((file = open_file(arg_cpy, &fsize)))
		{
			if (ft_woody(file, arg_cpy[1], fsize, arg_cpy[2]))
			{
				free_sp(arg_cpy);
				return (1);
			}
		}
	}
	else
		printf("woody_woodpacker: Too many arguments\n");
	free_sp(arg_cpy);
	return (0);
}