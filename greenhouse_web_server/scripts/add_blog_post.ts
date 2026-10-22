import { prisma } from "../lib/prisma";

async function main() {
  const user = await prisma.post.create({
    data: {
      title: "A really great post",
      content: "Theres really not much more to say here other than its a graet post and everyone should read it"
    },
  });
  console.log("Created user:", user);
}

main()
  .then(async () => {
    await prisma.$disconnect();
  })
  .catch(async (e) => {
    console.error(e);
    await prisma.$disconnect();
    process.exit(1);
  });