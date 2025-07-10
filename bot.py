import discord
from discord.ext import commands
import os
from dotenv import load_dotenv
import asyncio

# Load environment variables
load_dotenv()

# Bot configurations
BOT1_TOKEN = os.getenv('DISCORD_TOKEN_1')
BOT2_TOKEN = os.getenv('DISCORD_TOKEN_2')

if not BOT1_TOKEN or not BOT2_TOKEN:
    raise ValueError("Both DISCORD_TOKEN_1 and DISCORD_TOKEN_2 environment variables are required")

# Create intents
intents = discord.Intents.default()
intents.dm_messages = True
intents.message_content = True

# Bot 1 - Phoebe
bot1 = commands.Bot(command_prefix='!bot1', intents=intents)
bot1_user_states = {}

# Bot 2 - Cryptix
bot2 = commands.Bot(command_prefix='!bot2', intents=intents)
bot2_user_states = {}

# Bot 1 Events
@bot1.event
async def on_ready():
    print(f"✅ Bot1 (Phoebe) is online as {bot1.user}")

@bot1.event
async def on_message(message):
    if message.author == bot1.user:
        return
    
    if isinstance(message.channel, discord.DMChannel):
        user_id = message.author.id
        
        if user_id not in bot1_user_states and message.content.strip().lower() != 'how you doin\'':
            return
        
        if user_id not in bot1_user_states:
            bot1_user_states[user_id] = 'awaiting_password'
            await message.channel.send("Phoebe 🎤")
        else:
            if message.content.strip() == 'smellycat':
                await message.channel.send(f"✅ Correct! Your word is: ilove")
                del bot1_user_states[user_id]
            else:
                await message.channel.send("❌ Wrong password. Try again.")
    
    await bot1.process_commands(message)

# Bot 2 Events
@bot2.event
async def on_ready():
    print(f"✅ Bot2 (Cryptix) is online as {bot2.user}")

@bot2.event
async def on_message(message):
    if message.author == bot2.user:
        return
    
    if isinstance(message.channel, discord.DMChannel):
        user_id = message.author.id
        
        if user_id not in bot2_user_states and message.content.strip().lower() != 'how you doin\'':
            return
        
        if user_id not in bot2_user_states:
            bot2_user_states[user_id] = 'awaiting_password'
            await message.channel.send("What was it almost getting called")
        else:
            if message.content.strip() == 'insomniacafe':
                await message.channel.send(f"✅ Correct! Your word is: cryptix")
                del bot2_user_states[user_id]
            else:
                await message.channel.send("❌ Wrong password. Try again.")
    
    await bot2.process_commands(message)

async def run_bots():
    """Run both bots concurrently"""
    await asyncio.gather(
        bot1.start(BOT1_TOKEN),
        bot2.start(BOT2_TOKEN)
    )

if __name__ == "__main__":
    asyncio.run(run_bots())
