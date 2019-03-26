from PIL import Image

i = j = 499

im = Image.open('Images/512/forest_color.jpg')
rgb_im = im.convert('RGB')
r, g, b = rgb_im.getpixel((0, 0))
print(r, g, b)
r, g, b = rgb_im.getpixel((0, 1))
print(r, g, b)
r, g, b = rgb_im.getpixel((0, 2))
print(r, g, b)

